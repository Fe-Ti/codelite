#include "PHPEntityBase.h"

PHPEntityBase::PHPEntityBase()
    : m_parent(NULL)
    , m_line(0)
    , m_column(0)
    , m_dbId(wxNOT_FOUND)
{
}

void PHPEntityBase::AddChild(PHPEntityBase::Ptr_t child)
{
    if(m_childrenMap.count(child->GetName()) == 0) {
        m_children.push_back(child);
        m_childrenMap.insert(std::make_pair(child->GetName(), child));
        child->m_parent = this;
    }        
}

void PHPEntityBase::RecursivePrintStdout(PHPEntityBase::Ptr_t parent, int indent) { PrintStdout(indent); }

PHPEntityBase::Ptr_t PHPEntityBase::FindChild(const wxString& name, bool tryPrependingDollar) const
{
    PHPEntityBase::Map_t::const_iterator iter = m_childrenMap.find(name);
    if(iter != m_childrenMap.end()) {
        return iter->second;
    }

    // Could not find an exact match, try prepending
    if(tryPrependingDollar) {
        wxString modName = name;
        if(!modName.StartsWith("$")) {
            modName.Prepend("$");
        }
        iter = m_childrenMap.find(modName);
        if(iter != m_childrenMap.end()) {
            return iter->second;
        }
    }
    return PHPEntityBase::Ptr_t(NULL);
}

void PHPEntityBase::StoreRecursive(wxSQLite3Database& db)
{
    Store(db);
    // save the children
    PHPEntityBase::List_t::iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        (*iter)->StoreRecursive(db);
    }
}
