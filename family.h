#ifndef FAMILY_H
#define FAMILY_H

#include "Person.h"
#include <istream>
#include <set>
#include <string>
#include <map>

class GenePool {
  // Member Variables
  std::map<std::string, Person*> m_people;

  // Helper Functions
  void addPerson(const std::string& name, Gender gender, const std::string& motherName, const std::string& fatherName);

public:
  // Build a database of people from a TSV file.
  GenePool(std::istream& stream);

  // Clean it up.
  ~GenePool();

  // List all the people in the database.
  std::set<Person*> everyone() const;

  // Find a person in the database by name.
  // Return nullptr if there is no such person.
  Person* find(const std::string& name) const;
};

#endif
