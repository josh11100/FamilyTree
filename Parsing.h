#ifndef PARSING_H
#define PARSING_H

#include "Roles.h"
#include "family.h"
#include "Person.h"


class Query {
  std::string mName;
  std::string mRelationship;
  PMod        mPMod;
  SMod        mSMod;

  void validate() const;
  void validate(bool allow_pmod, bool allow_smod) const;

public:
  Query(const std::string& text);
  Query(
    const std::string& name,
    const std::string& relationship,
    PMod pmod = PMod::ANY,
    SMod smod = SMod::ANY
  );

  std::set<Person*> run(const GenePool& pool) const;
  std::string to_string() const;
};

#endif

