
#include "family.h"
// family Member Functions
#include <sstream>

// Destructor
GenePool::~GenePool() {
    for (auto& pair : m_people) {
        delete pair.second;
    }
}

// Constructor
GenePool::GenePool(std::istream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string name, gender, motherName, fatherName;
        std::getline(ss, name, '\t');
        std::getline(ss, gender, '\t');
        std::getline(ss, motherName, '\t');
        std::getline(ss, fatherName, '\t');

        addPerson(name, gender == "male" ? Gender::MALE : Gender::FEMALE, motherName, fatherName);
    }
}

// Adding a person into the family tree data base / Updating information
void GenePool::addPerson(const std::string& name, Gender gender, const std::string& motherName, const std::string& fatherName) {
    Person* mother = (motherName != "???") ? find(motherName) : nullptr;
    Person* father = (fatherName != "???") ? find(fatherName) : nullptr;
    Person* person = new Person(name, gender, mother, father);
    m_people[name] = person;

    if (mother) mother->m_children.insert(person);
    if (father) father->m_children.insert(person);
}

// Listing everyone in the database/Tree
std::set<Person*> GenePool::everyone() const {
    std::set<Person*> result;
    for (const auto& pair : m_people) {
        result.insert(pair.second);
    }
    return result;
}

// Locate the person in the database/family Tree 
Person* GenePool::find(const std::string& name) const {
    auto it = m_people.find(name);
    return it != m_people.end() ? it->second : nullptr;
}