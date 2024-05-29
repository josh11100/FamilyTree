#include "Person.h"
#include <iostream>

// Constructor
Person::Person(const std::string& name, Gender gender, Person* mother, Person* father)
    : m_name(name), m_gender(gender), m_mother(mother), m_father(father) {
    if (m_mother) {
        m_mother->m_children.insert(this);
    }
    if (m_father) {
        m_father->m_children.insert(this);
    }
}

// Getter Functions
const std::string& Person::name() const {
    return m_name;
}

Gender Person::gender() const {
    return m_gender;
}

Person* Person::mother() {
    return m_mother;
}

Person* Person::father() {
    return m_father;
}

// Relationship Functions
/*
The ancestors function finds all ancestors of a person recursively.
It includes parents, grandparents, great-grandparents, etc.
*/
std::set<Person*> Person::ancestors(PMod pmod) {
    std::set<Person*> result;
    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother) {
            result.insert(m_mother);
            auto maternalAncestors = m_mother->ancestors(PMod::ANY);
            result.insert(maternalAncestors.begin(), maternalAncestors.end());
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father) {
            result.insert(m_father);
            auto paternalAncestors = m_father->ancestors(PMod::ANY);
            result.insert(paternalAncestors.begin(), paternalAncestors.end());
        }
    }
    return result;
}

/*
The aunts function finds all aunts of a person based on the parent and sibling modifiers.
It includes both maternal and paternal aunts.
*/
std::set<Person*> Person::aunts(PMod pmod, SMod smod) {
    std::set<Person*> result;
    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother) {
            auto siblings = m_mother->siblings(PMod::ANY, smod);
            for (Person* sibling : siblings) {
                if (sibling->gender() == Gender::FEMALE) {
                    result.insert(sibling);
                }
            }
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father) {
            auto siblings = m_father->siblings(PMod::ANY, smod);
            for (Person* sibling : siblings) {
                if (sibling->gender() == Gender::FEMALE) {
                    result.insert(sibling);
                }
            }
        }
    }
    return result;
}

/*
The uncles function finds all uncles of a person based on the parent and sibling modifiers.
It includes both maternal and paternal uncles.
*/
std::set<Person*> Person::uncles(PMod pmod, SMod smod) {
    std::set<Person*> result;
    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother) {
            auto siblings = m_mother->siblings(PMod::ANY, smod);
            for (Person* sibling : siblings) {
                if (sibling->gender() == Gender::MALE) {
                    result.insert(sibling);
                }
            }
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father) {
            auto siblings = m_father->siblings(PMod::ANY, smod);
            for (Person* sibling : siblings) {
                if (sibling->gender() == Gender::MALE) {
                    result.insert(sibling);
                }
            }
        }
    }
    return result;
}

/*
The brothers function finds all brothers of a person based on the parent and sibling modifiers.
It includes both full and half brothers.
*/
std::set<Person*> Person::brothers(PMod pmod, SMod smod) {
    std::set<Person*> result;
    std::set<Person*> siblingsSet;

    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother) {
            for (Person* sibling : m_mother->children()) {
                if (sibling != this) {
                    siblingsSet.insert(sibling);
                }
            }
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father) {
            for (Person* sibling : m_father->children()) {
                if (sibling != this) {
                    siblingsSet.insert(sibling);
                }
            }
        }
    }

    for (Person* sibling : siblingsSet) {
        if (sibling->gender() == Gender::MALE) {
            bool isFullBrother = (sibling->mother() && sibling->mother() == this->mother() && sibling->father() && sibling->father() == this->father());
            bool isMaternalHalfBrother = (sibling->mother() && sibling->mother() == this->mother() && (!sibling->father() || sibling->father() != this->father()));
            bool isPaternalHalfBrother = (sibling->father() && sibling->father() == this->father() && (!sibling->mother() || sibling->mother() != this->mother()));

            if (smod == SMod::FULL) {
                if (isFullBrother) {
                    result.insert(sibling);
                }
            } else if (smod == SMod::HALF) {
                if ((pmod == PMod::MATERNAL && isMaternalHalfBrother) ||
                    (pmod == PMod::PATERNAL && isPaternalHalfBrother) ||
                    (pmod == PMod::ANY && (isMaternalHalfBrother || isPaternalHalfBrother))) {
                    result.insert(sibling);
                }
            } else if (smod == SMod::ANY) {
                if ((pmod == PMod::MATERNAL && (isFullBrother or isMaternalHalfBrother)) ||
                    (pmod == PMod::PATERNAL && (isFullBrother or isPaternalHalfBrother)) ||
                    (pmod == PMod::ANY && (isFullBrother or isMaternalHalfBrother or isPaternalHalfBrother))) {
                    result.insert(sibling);
                }
            }
        }
    }

    return result;
}

/*
The children function returns all children of a person.
*/
std::set<Person*> Person::children() {
    return m_children;
}

/*
The cousins function should find all the children of the person's aunts and uncles.
The function uses the aunts and uncles functions to get the aunts and uncles, 
then adds their children to the result set.
*/
std::set<Person*> Person::cousins(PMod pmod, SMod smod) {
    std::set<Person*> result;

    // Get aunts and add their children
    for (Person* person : aunts(pmod, smod)) {
        auto personChildren = person->children();
        result.insert(personChildren.begin(), personChildren.end());
    }

    // Get uncles and add their children
    for (Person* p : uncles(pmod, smod)) {
        auto pChildren = p->children();
        result.insert(pChildren.begin(), pChildren.end());
    }

    return result;
}

/*
The daughters function filters children to only include female children.
*/
std::set<Person*> Person::daughters() {
    std::set<Person*> result;
    for (Person* child : m_children) {
        if (child->gender() == Gender::FEMALE) {
            result.insert(child);
        }
    }
    return result;
}

/*
The descendants function finds all descendants of a person recursively.
It includes children, grandchildren, great-grandchildren, etc.
*/
std::set<Person*> Person::descendants() {
    std::set<Person*> result;
    for (Person* child : m_children) {
        result.insert(child);
        auto childDescendants = child->descendants();
        result.insert(childDescendants.begin(), childDescendants.end());
    }
    return result;
}

/*
The grandchildren function finds all grandchildren of a person.
It gets the children of each child and adds them to the result set.
*/
std::set<Person*> Person::grandchildren() {
    std::set<Person*> result;
    for (Person* child : m_children) {
        auto grandchildren = child->children();
        result.insert(grandchildren.begin(), grandchildren.end());
    }
    return result;
}

/*
The granddaughters function finds all granddaughters of a person.
It filters grandchildren to only include female grandchildren.
*/
std::set<Person*> Person::granddaughters() {
    std::set<Person*> result;
    auto grandchildren = this->grandchildren();
    for (Person* grandchild : grandchildren) {
        if (grandchild->gender() == Gender::FEMALE) {
            result.insert(grandchild);
        }
    }
    return result;
}

/*
The grandfathers function finds all grandfathers of a person based on the parent modifier.
It includes both maternal and paternal grandfathers.
*/
std::set<Person*> Person::grandfathers(PMod pmod) {
    std::set<Person*> result;
    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother && m_mother->father()) {
            result.insert(m_mother->father());
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father && m_father->father()) {
            result.insert(m_father->father());
        }
    }
    return result;
}

/*
The grandmothers function finds all grandmothers of a person based on the parent modifier.
It includes both maternal and paternal grandmothers.
*/
std::set<Person*> Person::grandmothers(PMod pmod) {
    std::set<Person*> result;
    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother && m_mother->mother()) {
            result.insert(m_mother->mother());
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father && m_father->mother()) {
            result.insert(m_father->mother());
        }
    }
    return result;
}

/*
The grandparents function finds all grandparents of a person.
It combines the results of grandfathers and grandmothers functions.
*/
std::set<Person*> Person::grandparents(PMod pmod) {
    std::set<Person*> result;
    auto grandfathers = this->grandfathers(pmod);
    auto grandmothers = this->grandmothers(pmod);
    result.insert(grandfathers.begin(), grandfathers.end());
    result.insert(grandmothers.begin(), grandmothers.end());
    return result;
}

/*
The grandsons function finds all grandsons of a person.
It filters grandchildren to only include male grandchildren.
*/
std::set<Person*> Person::grandsons() {
    std::set<Person*> result;
    auto grandchildren = this->grandchildren();
    for (Person* grandchild : grandchildren) {
        if (grandchild->gender() == Gender::MALE) {
            result.insert(grandchild);
        }
    }
    return result;
}

/*
The nephews function finds all nephews of a person.
It gets the children of each sibling and filters them to only include male children.
*/
std::set<Person*> Person::nephews(PMod pmod, SMod smod) {
    std::set<Person*> result;
    auto siblingsSet = this->siblings(pmod, smod);

    for (Person* sibling : siblingsSet) {
        for (Person* child : sibling->children()) {
            if (child->gender() == Gender::MALE) {
                if (smod == SMod::FULL) {
                    bool isFullNephew = (sibling->mother() && sibling->mother() == this->mother() && sibling->father() && sibling->father() == this->father());
                    if (isFullNephew) {
                        result.insert(child);
                    }
                } else if (smod == SMod::HALF) {
                    bool isMaternalHalfNephew = (sibling->mother() && sibling->mother() == this->mother() && (!sibling->father() || sibling->father() != this->father()));
                    bool isPaternalHalfNephew = (sibling->father() && sibling->father() == this->father() && (!sibling->mother() || sibling->mother() != this->mother()));
                    if ((pmod == PMod::MATERNAL && isMaternalHalfNephew) ||
                        (pmod == PMod::PATERNAL && isPaternalHalfNephew) ||
                        (pmod == PMod::ANY && (isMaternalHalfNephew || isPaternalHalfNephew))) {
                        result.insert(child);
                    }
                } else if (smod == SMod::ANY) {
                    bool isFullNephew = (sibling->mother() && sibling->mother() == this->mother() && sibling->father() && sibling->father() == this->father());
                    bool isMaternalHalfNephew = (sibling->mother() && sibling->mother() == this->mother() && (!sibling->father() || sibling->father() != this->father()));
                    bool isPaternalHalfNephew = (sibling->father() && sibling->father() == this->father() && (!sibling->mother() || sibling->mother() != this->mother()));
                    if ((pmod == PMod::MATERNAL && (isFullNephew or isMaternalHalfNephew)) ||
                        (pmod == PMod::PATERNAL && (isFullNephew or isPaternalHalfNephew)) ||
                        (pmod == PMod::ANY && (isFullNephew or isMaternalHalfNephew or isPaternalHalfNephew))) {
                        result.insert(child);
                    }
                }
            }
        }
    }

    return result;
}

/*
The nieces function finds all nieces of a person.
It gets the children of each sibling and filters them to only include female children.
*/
std::set<Person*> Person::nieces(PMod pmod, SMod smod) {
    std::set<Person*> result;
    auto siblingsSet = this->siblings(pmod, smod);

    for (Person* sibling : siblingsSet) {
        for (Person* child : sibling->children()) {
            if (child->gender() == Gender::FEMALE) {
                if (smod == SMod::FULL) {
                    bool isFullNiece = (sibling->mother() && sibling->mother() == this->mother() && sibling->father() && sibling->father() == this->father());
                    if (isFullNiece) {
                        result.insert(child);
                    }
                } else if (smod == SMod::HALF) {
                    bool isMaternalHalfNiece = (sibling->mother() && sibling->mother() == this->mother() && (!sibling->father() || sibling->father() != this->father()));
                    bool isPaternalHalfNiece = (sibling->father() && sibling->father() == this->father() && (!sibling->mother() || sibling->mother() != this->mother()));
                    if ((pmod == PMod::MATERNAL && isMaternalHalfNiece) ||
                        (pmod == PMod::PATERNAL && isPaternalHalfNiece) ||
                        (pmod == PMod::ANY && (isMaternalHalfNiece or isPaternalHalfNiece))) {
                        result.insert(child);
                    }
                } else if (smod == SMod::ANY) {
                    bool isFullNiece = (sibling->mother() && sibling->mother() == this->mother() && sibling->father() && sibling->father() == this->father());
                    bool isMaternalHalfNiece = (sibling->mother() && sibling->mother() == this->mother() && (!sibling->father() || sibling->father() != this->father()));
                    bool isPaternalHalfNiece = (sibling->father() && sibling->father() == this->father() && (!sibling->mother() || sibling->mother() != this->mother()));
                    if ((pmod == PMod::MATERNAL && (isFullNiece or isMaternalHalfNiece)) ||
                        (pmod == PMod::PATERNAL && (isFullNiece or isPaternalHalfNiece)) ||
                        (pmod == PMod::ANY && (isFullNiece or isMaternalHalfNiece or isPaternalHalfNiece))) {
                        result.insert(child);
                    }
                }
            }
        }
    }

    return result;
}

/*
The parents function finds the parents of a person based on the parent modifier.
It includes both the mother and father.
*/
std::set<Person*> Person::parents(PMod pmod) {
    std::set<Person*> result;
    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother) {
            result.insert(m_mother);
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father) {
            result.insert(m_father);
        }
    }
    return result;
}

/*
The siblings function finds all siblings of a person based on the parent and sibling modifiers.
It includes both full and half siblings based on the sibling modifier.
*/
std::set<Person*> Person::siblings(PMod pmod, SMod smod) {
    std::set<Person*> result;
    std::set<Person*> maternalSiblings;
    std::set<Person*> paternalSiblings;

    if (m_mother) {
        maternalSiblings = m_mother->children();
    }
    if (m_father) {
        paternalSiblings = m_father->children();
    }

    for (Person* sibling : maternalSiblings) {
        if (sibling != this) {
            bool isFullSibling = paternalSiblings.count(sibling) > 0;
            if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
                if (smod == SMod::ANY || (smod == SMod::FULL && isFullSibling) || (smod == SMod::HALF && !isFullSibling)) {
                    result.insert(sibling);
                }
            }
        }
    }

    for (Person* sibling : paternalSiblings) {
        if (sibling != this) {
            bool isFullSibling = maternalSiblings.count(sibling) > 0;
            if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
                if (smod == SMod::ANY || (smod == SMod::FULL && isFullSibling) || (smod == SMod::HALF && !isFullSibling)) {
                    result.insert(sibling);
                }
            }
        }
    }

    return result;
}

/*
The sisters function finds all sisters of a person based on the parent and sibling modifiers.
It filters siblings to only include female siblings.
*/
std::set<Person*> Person::sisters(PMod pmod, SMod smod) {
    std::set<Person*> result;
    std::set<Person*> siblingsSet;

    if (pmod == PMod::ANY || pmod == PMod::MATERNAL) {
        if (m_mother) {
            for (Person* sibling : m_mother->children()) {
                if (sibling != this) {
                    siblingsSet.insert(sibling);
                }
            }
        }
    }
    if (pmod == PMod::ANY || pmod == PMod::PATERNAL) {
        if (m_father) {
            for (Person* sibling : m_father->children()) {
                if (sibling != this) {
                    siblingsSet.insert(sibling);
                }
            }
        }
    }

    for (Person* sibling : siblingsSet) {
        if (sibling->gender() == Gender::FEMALE) {
            bool isFullSister = (sibling->mother() && sibling->mother() == this->mother() && sibling->father() && sibling->father() == this->father());
            bool isMaternalHalfSister = (sibling->mother() && sibling->mother() == this->mother() && (!sibling->father() || sibling->father() != this->father()));
            bool isPaternalHalfSister = (sibling->father() && sibling->father() == this->father() && (!sibling->mother() || sibling->mother() != this->mother()));

            if (smod == SMod::FULL) {
                if (isFullSister) {
                    result.insert(sibling);
                }
            } else if (smod == SMod::HALF) {
                if ((pmod == PMod::MATERNAL && isMaternalHalfSister) ||
                    (pmod == PMod::PATERNAL && isPaternalHalfSister) ||
                    (pmod == PMod::ANY && (isMaternalHalfSister || isPaternalHalfSister))) {
                    result.insert(sibling);
                }
            } else if (smod == SMod::ANY) {
                if ((pmod == PMod::MATERNAL && (isFullSister or isMaternalHalfSister)) ||
                    (pmod == PMod::PATERNAL && (isFullSister or isPaternalHalfSister)) ||
                    (pmod == PMod::ANY && (isFullSister or isMaternalHalfSister or isPaternalHalfSister))) {
                    result.insert(sibling);
                }
            }
        }
    }

    return result;
}

/*
The sons function filters children to only include male children.
*/
std::set<Person*> Person::sons() {
    std::set<Person*> result;
    for (Person* child : m_children) {
        if (child->gender() == Gender::MALE) {
            result.insert(child);
        }
    }
    return result;
}
