#include <osg/Node>
#include <osg/Group>
#include <osg/NodeVisitor>

#include <osg/Notify>

#include <algorithm>

using namespace osg;

Node::Node()
{
    _bsphere_computed = false;
    _userData = NULL;
    _nodeMask = 0xffffffff;
    
    _numChildrenRequiringAppTraversal = 0;

    _cullingActive = true;
    _numChildrenWithCullingDisabled = 0;

}


Node::~Node()
{
    if (_userData && _memoryAdapter.valid()) _memoryAdapter->unref_data(_userData);
}


void Node::accept(NodeVisitor& nv)
{
    if (nv.validNodeMask(*this)) nv.apply(*this);
}


void Node::ascend(NodeVisitor& nv)
{
    std::for_each(_parents.begin(),_parents.end(),NodeAcceptOp(nv));
}

void Node::setAppCallback(NodeCallback* nc)
{
    // if no changes just return.
    if (_appCallback==nc) return;
    
    // app callback has been changed, will need to update
    // both _appCallback and possibly the numChildrenRequiringAppTraversal
    // if the number of callbacks changes.


    // update the parents numChildrenRequiringAppTraversal
    // note, if _numChildrenRequiringAppTraversal!=0 then the
    // parents won't be affected by any app callback change,
    // so no need to inform them.
    if (_numChildrenRequiringAppTraversal==0 && !_parents.empty())
    {
        int delta = 0;
        if (_appCallback.valid()) --delta;
        if (nc) ++delta;
        if (delta!=0)
        {
            // the number of callbacks has changed, need to pass this
            // on to parents so they know whether app traversal is
            // reqired on this subgraph.
            for(ParentList::iterator itr =_parents.begin();
                itr != _parents.end();
                ++itr)
            {    
                (*itr)->setNumChildrenRequiringAppTraversal(
                        (*itr)->getNumChildrenRequiringAppTraversal()+delta );
            }

        }
    }

    // set the app callback itself.
    _appCallback = nc;

}

void Node::setNumChildrenRequiringAppTraversal(const int num)
{
    // if no changes just return.
    if (_numChildrenRequiringAppTraversal==num) return;

    // note, if _appCallback is set then the
    // parents won't be affected by any changes to
    // _numChildrenRequiringAppTraversal so no need to inform them.
    if (!_appCallback && !_parents.empty())
    {
    
        // need to pass on changes to parents.        
        int delta = 0;
        if (_numChildrenRequiringAppTraversal>0) --delta;
        if (num>0) ++delta;
        if (delta!=0)
        {
            // the number of callbacks has changed, need to pass this
            // on to parents so they know whether app traversal is
            // reqired on this subgraph.
            for(ParentList::iterator itr =_parents.begin();
                itr != _parents.end();
                ++itr)
            {    
                (*itr)->setNumChildrenRequiringAppTraversal(
                    (*itr)->getNumChildrenRequiringAppTraversal()+delta
                    );
            }

        }
    }
    
    // finally update this objects value.
    _numChildrenRequiringAppTraversal=num;
    
}

void Node::setCullingActive(const bool active)
{
    // if no changes just return.
    if (_cullingActive == active) return;
    
    // culling active has been changed, will need to update
    // both _cullActive and possibly the parents numChildrenWithCullingDisabled
    // if culling disabled changes.

    // update the parents _numChildrenWithCullingDisabled
    // note, if _numChildrenWithCullingDisabled!=0 then the
    // parents won't be affected by any app callback change,
    // so no need to inform them.
    if (_numChildrenWithCullingDisabled==0 && !_parents.empty())
    {
        int delta = 0;
        if (!_cullingActive) --delta;
        if (!active) ++delta;
        if (delta!=0)
        {
            // the number of callbacks has changed, need to pass this
            // on to parents so they know whether app traversal is
            // reqired on this subgraph.
            for(ParentList::iterator itr =_parents.begin();
                itr != _parents.end();
                ++itr)
            {    
                (*itr)->setNumChildrenWithCullingDisabled(
                        (*itr)->getNumChildrenWithCullingDisabled()+delta );
            }

        }
    }

    // set the cullingActive itself.
    _cullingActive = active;
}

void Node::setNumChildrenWithCullingDisabled(const int num)
{
    // if no changes just return.
    if (_numChildrenWithCullingDisabled==num) return;

    // note, if _cullingActive is false then the
    // parents won't be affected by any changes to
    // _numChildrenWithCullingDisabled so no need to inform them.
    if (_cullingActive && !_parents.empty())
    {
    
        // need to pass on changes to parents.        
        int delta = 0;
        if (_numChildrenWithCullingDisabled>0) --delta;
        if (num>0) ++delta;
        if (delta!=0)
        {
            // the number of callbacks has changed, need to pass this
            // on to parents so they know whether app traversal is
            // reqired on this subgraph.
            for(ParentList::iterator itr =_parents.begin();
                itr != _parents.end();
                ++itr)
            {    
                (*itr)->setNumChildrenWithCullingDisabled(
                    (*itr)->getNumChildrenWithCullingDisabled()+delta
                    );
            }

        }
    }
    
    // finally update this objects value.
    _numChildrenWithCullingDisabled=num;
}


const bool Node::computeBound() const
{
    _bsphere.init();
    return false;
}


void Node::dirtyBound()
{
    if (_bsphere_computed)
    {
        _bsphere_computed = false;

        // dirty parent bounding sphere's to ensure that all are valid.
        for(ParentList::iterator itr=_parents.begin();
            itr!=_parents.end();
            ++itr)
        {
            (*itr)->dirtyBound();
        }

    }
}
