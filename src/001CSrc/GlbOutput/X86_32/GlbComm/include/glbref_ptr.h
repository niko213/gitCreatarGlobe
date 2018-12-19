#pragma once
template<class T>
class glbref_ptr
{
public:
	typedef T element_type;
	glbref_ptr(void):mpr_ptr(0){}
	glbref_ptr(T* ptr) : mpr_ptr(ptr)
	{
		if (mpr_ptr) mpr_ptr->Ref();
	}
	glbref_ptr(const glbref_ptr& rp) : mpr_ptr(rp.mpr_ptr) { if (mpr_ptr) mpr_ptr->Ref(); }
	template<class Other> glbref_ptr(const glbref_ptr<Other>& rp) : mpr_ptr(rp.mpr_ptr)
	{
		if (mpr_ptr) mpr_ptr->Ref();
	}

	~glbref_ptr(void)
	{
		if (mpr_ptr)
			mpr_ptr->UnRef();
		mpr_ptr = 0;
	}
	
	glbref_ptr& operator = (const glbref_ptr& rp)
	{
		assign(rp);
		return *this;
	}

	template<class Other> glbref_ptr& operator = (const glbref_ptr<Other>& rp)
	{
		assign(rp);
		return *this;
	}

	inline glbref_ptr& operator = (T* ptr)
	{
		if (mpr_ptr==ptr) return *this;
		T* tmp_ptr = mpr_ptr;
		mpr_ptr = ptr;
		if (mpr_ptr) mpr_ptr->Ref();
		if (tmp_ptr) tmp_ptr->UnRef();
		return *this;
	}
#ifdef GLBCOMM_USE_REF_PTR_IMPLICIT_OUTPUT_CONVERSION
	/*
	   类型转换操作符	   
	   void Fun(A *r1,B *r2){}
	   void Fun2()
	   {
			ref_ptr<A> p1,p2;
			Fun(p1,p2);---- 隐式类型转换.
			int a = p1 - p2;//可以编译通过(隐式转换为指针),会产生不可预料的错误!!!!!.
	   }

	   operator T *()
	   {
			return mpr_ptr;
	   }
	   
	   operator const T *()const
	   {
	   		return mpr_ptr;
	   }
	*/
#else	
	bool operator == (const glbref_ptr& rp) const { return (mpr_ptr==rp.mpr_ptr); }
    bool operator == (const T* ptr) const { return (mpr_ptr==ptr); }
    friend bool operator == (const T* ptr, const glbref_ptr& rp) { return (ptr==rp.mpr_ptr); }

	bool operator != (const glbref_ptr& rp) const { return (mpr_ptr!=rp.mpr_ptr); }
	bool operator != (const T* ptr) const { return (mpr_ptr!=ptr); }
	friend bool operator != (const T* ptr, const glbref_ptr& rp) { return (ptr!=rp.mpr_ptr); }

	bool operator < (const glbref_ptr& rp) const { return (mpr_ptr<rp.mpr_ptr); }

	private:
        typedef T* glbref_ptr::*unspecified_bool_type;
    public:
        /* 
			safe bool conversion
			执行if (ptr)时候便会执行operator unspecified_bool_type() const,
			转嫁成判断unspecified_bool_type类型的指针是否为空
		*/
        operator unspecified_bool_type() const { return valid()? &glbref_ptr::mpr_ptr : 0; }
#endif
	T& operator*() const { return *mpr_ptr; }
    T* operator->() const { return mpr_ptr; }
	T* get() const { return mpr_ptr; }

	bool operator!() const   { return mpr_ptr==0; } // not required
	bool valid() const       { return mpr_ptr!=0; }

    T* release()
	{
		T* tmp=mpr_ptr;
		if (mpr_ptr) mpr_ptr->UnRef_Nodelete();
		mpr_ptr=0;
		return tmp;
	}

	void swap(glbref_ptr& rp) { T* tmp=mpr_ptr; mpr_ptr=rp.mpr_ptr; rp.mpr_ptr=tmp; }
private:
	template<class Other> void assign(const glbref_ptr<Other>& rp)
    {
		if (mpr_ptr==rp.mpr_ptr) return;
		T* tmp_ptr = mpr_ptr;
		mpr_ptr = rp.mpr_ptr;
		if (mpr_ptr) mpr_ptr->Ref();
        if (tmp_ptr) tmp_ptr->UnRef();
	}

    template<class Other> friend class glbref_ptr;
	T* mpr_ptr;
};

#include "GlbObServerSet.h"
template<class T>
class glbobserver_ptr
{
public:
	typedef T element_type;

	glbobserver_ptr() : mpr_observerSet(0), mpr_ptr(0) {}

	glbobserver_ptr(T* rp)
	{
		mpr_observerSet = rp? rp->GetObserverSet() : 0;
		mpr_ptr = (mpr_observerSet.valid() && mpr_observerSet->GetObserverdObject()!=0) ? rp : 0;
	}

	~glbobserver_ptr(){}

	glbobserver_ptr(const glbref_ptr<T>& rp)
    {
        mpr_observerSet = rp.valid() ? rp->GetObserverSet() : 0;
        mpr_ptr = (mpr_observerSet.valid() && mpr_observerSet->GetObserverdObject()!=0) ? rp.get() : 0;
    }

	glbobserver_ptr(const glbobserver_ptr& wp) :
        mpr_observerSet(wp.mpr_observerSet),
        mpr_ptr(wp.mpr_ptr)
    {
    }
	
	glbobserver_ptr& operator = (const glbobserver_ptr& wp)
    {
        if (&wp==this) return *this;

        mpr_observerSet = wp.mpr_observerSet;
        mpr_ptr = wp.mpr_ptr;
        return *this;
    }

    glbobserver_ptr& operator = (const glbref_ptr<T>& rp)
    {
        mpr_observerSet = rp.valid() ? rp->getOrCreateObserverSet() : 0;
        mpr_ptr = (mpr_observerSet.valid() && mpr_observerSet->GetObserverdObject()!=0) ? rp.get() : 0;
        return *this;
    }

    glbobserver_ptr& operator = (T* rp)
    {
        mpr_observerSet = rp ? rp->getOrCreateObserverSet() : 0;
        mpr_ptr = (mpr_observerSet.valid() && mpr_observerSet->getObserverdObject()!=0) ? rp : 0;
        return *this;
    }

	bool operator == (const glbobserver_ptr& wp) const { return mpr_observerSet == wp.mpr_observerSet; }
    bool operator != (const glbobserver_ptr& wp) const { return mpr_observerSet != wp.mpr_observerSet; }
    bool operator < (const glbobserver_ptr& wp) const { return mpr_observerSet < wp.mpr_observerSet; }
    bool operator > (const glbobserver_ptr& wp) const { return mpr_observerSet > wp.mpr_observerSet; }

	inline bool operator == (const T* ptr) const { return mpr_ptr == ptr; }
    inline bool operator != (const T* ptr) const { return mpr_ptr != ptr; }
    inline bool operator < (const T* ptr) const { return mpr_ptr < ptr; }
    inline bool operator > (const T* ptr) const { return mpr_ptr > ptr; }

	inline T& operator*() const { return *mpr_ptr; }
    inline T* operator->() const { return mpr_ptr; }

	inline T* get() const { return (mpr_observerSet.valid() && mpr_observerSet->GetObserverdObject()!=0) ? mpr_ptr : 0; }

    inline bool operator!() const   { return get() == 0; }
    inline bool valid() const       { return get() != 0; }

	/*
	*   安全获取指针
	*/
	bool SafeGet(glbref_ptr<T>& rptr) const
    {
        if (!mpr_observerSet)
        {
            rptr = 0;
            return false;
        }

        CGlbReference* obj = mpr_observerSet->AddRefLock();
        if (!obj)
        {
            rptr = 0;
            return false;
        }

        rptr = mpr_ptr;
        obj->UnRef_Nodelete();
        return rptr.valid();
    }
private:
	glbref_ptr<CGlbObserverSet> mpr_observerSet;
	T* mpr_ptr;
};
