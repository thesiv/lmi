// Fund data.
//
// Copyright (C) 1998, 2001, 2002, 2003, 2005 Gregory W. Chicares.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// http://savannah.nongnu.org/projects/lmi
// email: <chicares@cox.net>
// snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

// $Id: ihs_funddata.hpp,v 1.6 2005-12-22 13:59:49 chicares Exp $

#ifndef funddata_hpp
#define funddata_hpp

#include "config.hpp"

#include "obstruct_slicing.hpp"
#include "so_attributes.hpp"

#include <boost/utility.hpp>

#include <string>
#include <vector>

// Separate account funds: their names and investment mgmt fees

// Implicitly-declared special member functions do the right thing.

class LMI_SO FundInfo
    :virtual private obstruct_slicing<FundInfo>
{
    friend class FundData;

  public:
    FundInfo();
    FundInfo
        (double      ScalarIMF
        ,std::string ShortName
        ,std::string LongName
        );
    ~FundInfo();

    double ScalarIMF() const;
    std::string const& ShortName() const;
    std::string const& LongName() const;

  private:
    double ScalarIMF_;
    std::string ShortName_;
    std::string LongName_;
};

class LMI_SO FundData
    :private boost::noncopyable
    ,virtual private obstruct_slicing<FundData>
{
  public:
    FundData(std::string const& a_Filename);
    ~FundData();

    static void WriteFundFiles();
    static void WriteProprietaryFundFiles();

    FundInfo const& GetFundInfo(int j) const;
    int GetNumberOfFunds() const;

  private:
    FundData(); // Private, but implemented.

    void Read (std::string const& a_Filename);
    void Write(std::string const& a_Filename);

    std::vector<FundInfo> FundInfo_;
};

inline double FundInfo::ScalarIMF() const
{
    return ScalarIMF_;
}

inline std::string const& FundInfo::ShortName() const
{
    return ShortName_;
}

inline std::string const& FundInfo::LongName() const
{
    return LongName_;
}

inline FundInfo const& FundData::GetFundInfo(int j) const
{
    return FundInfo_[j];
}

inline int FundData::GetNumberOfFunds() const
{
    return FundInfo_.size();
}

#endif // funddata_hpp

