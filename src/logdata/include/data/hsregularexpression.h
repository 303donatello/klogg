/*
 * Copyright (C) 2021 Anton Filimonov and other contributors
 *
 * This file is part of klogg.
 *
 * klogg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * klogg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klogg.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KLOGG_HS_REGULAR_EXPRESSION
#define KLOGG_HS_REGULAR_EXPRESSION

#include <string_view>
#include <variant>
#include <vector>

#include <QRegularExpression>
#include <QString>

#ifdef KLOGG_HAS_HS
#include <hs.h>

#include "resourcewrapper.h"
#endif

#include "regularexpressionpattern.h"

using MatchedPatterns = std::vector<unsigned char>;
using MatchingResult = std::variant<bool, MatchedPatterns>;

class DefaultRegularExpressionMatcher {
  public:
    explicit DefaultRegularExpressionMatcher(
        const std::vector<RegularExpressionPattern>& patterns )
    {
        for ( const auto& pattern : patterns ) {
            regexp_.push_back( static_cast<QRegularExpression>( pattern ) );
        }
    }

    // std::vector<std::string> match( const QString& data ) const
    // {
    //     std::vector<std::string> matchingPatterns;
    //     for ( const auto& regexp : regexp_ ) {
    //         if ( regexp.second.match( data ).hasMatch() ) {
    //             matchingPatterns.push_back( regexp.first );
    //         }
    //     }
    //     return matchingPatterns;
    // }

    MatchingResult match( const std::string_view& utf8Data ) const
    {
        MatchedPatterns matchingPatterns;
        matchingPatterns.resize( regexp_.size() );
        for ( auto index = 0u; index < regexp_.size(); ++index ) {
            const auto hasMatch = regexp_[ index ]
                                      .match( QString::fromUtf8(
                                          utf8Data.data(), static_cast<int>( utf8Data.size() ) ) )
                                      .hasMatch();
            if ( regexp_.size() == 1 ) {
                return hasMatch;
            }

            matchingPatterns[ index ] = hasMatch;
        }

        return matchingPatterns;
    }

  private:
    std::vector<QRegularExpression> regexp_;
};

#ifdef KLOGG_HAS_HS

using HsScratch = UniqueResource<hs_scratch_t, hs_free_scratch>;
using HsDatabase = SharedResource<hs_database_t>;

class HsMatcher {
  public:
    HsMatcher() = default;
    HsMatcher( HsDatabase database, HsScratch scratch, const std::vector<std::string>& patternIds );

    HsMatcher( const HsMatcher& ) = delete;
    HsMatcher& operator=( const HsMatcher& ) = delete;

    HsMatcher( HsMatcher&& other ) = default;
    HsMatcher& operator=( HsMatcher&& other ) = default;

    MatchingResult match( const std::string_view& utf8Data ) const;

  private:
    HsDatabase database_;
    HsScratch scratch_;
    std::vector<std::string> patternIds_;
};

using MatcherVariant = std::variant<DefaultRegularExpressionMatcher, HsMatcher>;

class HsRegularExpression {
  public:
    HsRegularExpression() = default;
    explicit HsRegularExpression( const RegularExpressionPattern& includePattern );
    explicit HsRegularExpression( const std::vector<RegularExpressionPattern>& patterns );

    HsRegularExpression( const HsRegularExpression& ) = delete;
    HsRegularExpression& operator=( const HsRegularExpression& ) = delete;

    HsRegularExpression( HsRegularExpression&& other ) = default;
    HsRegularExpression& operator=( HsRegularExpression&& other ) = default;

    bool isValid() const;
    QString errorString() const;

    MatcherVariant createMatcher() const;

  private:
    bool isHsValid() const;

  private:
    HsDatabase database_;
    HsScratch scratch_;

    std::vector<RegularExpressionPattern> patterns_;
    std::vector<std::string> patternIds_;

    bool isValid_ = true;
    QString errorMessage_;
};
#else

using MatcherVariant = std::variant<DefaultRegularExpressionMatcher>;

class HsRegularExpression {
  public:
    HsRegularExpression() = default;

    explicit HsRegularExpression( const RegularExpressionPattern& includePattern )
        : HsRegularExpression( std::vector<RegularExpressionPattern>{ includePattern } )
    {
    }

    explicit HsRegularExpression( const std::vector<RegularExpressionPattern>& patterns )
        : patterns_( patterns )
    {
        for ( const auto& pattern : patterns_ ) {
            const auto& regex = static_cast<QRegularExpression>( pattern );
            if ( !regex.isValid() ) {
                isValid_ = false;
                errorString_ = regex.errorString();
                break;
            }
        }
    }

    bool isValid() const
    {
        return isValid_;
    }

    QString errorString() const
    {
        return errorString_;
    }

    MatcherVariant createMatcher() const
    {
        return MatcherVariant{ DefaultRegularExpressionMatcher( patterns_ ) };
    }

  private:
    bool isValid_ = true;
    QString errorString_;

    std::vector<RegularExpressionPattern> patterns_;
};

#endif

#endif