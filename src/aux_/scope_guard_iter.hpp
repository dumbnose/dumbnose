#define	N BOOST_PP_ITERATION()

#define	SCOPE_GUARD_CONST_PARAM_LIST(z,n,param)	BOOST_PP_COMMA() BOOST_PP_CAT(const	P,n)& BOOST_PP_CAT(p,n)
#define	SCOPE_GUARD_PARAM_INIT_LIST(z,n,param) BOOST_PP_COMMA() BOOST_PP_CAT(p,n)_ BOOST_PP_LPAREN() BOOST_PP_CAT(p,n) BOOST_PP_RPAREN()
#define	SCOPE_GUARD_CALL_PARAM_LIST(z,n,param) BOOST_PP_COMMA_IF(n) BOOST_PP_CAT(p,n)_
#define	SCOPE_GUARD_DECLARE_PARAM(z,n,param) BOOST_PP_CAT(const P,n) BOOST_PP_CAT(p,n)_;

/*	----------------------------------------------------------------------	*\
	scope_guard

	Type:		Non-member function
	# Params:	N
\*	----------------------------------------------------------------------	*/
template<typename Fun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class P)>
class BOOST_PP_CAT(scope_guard_impl, N)	: public scope_guard_impl_base
{
	Fun fun_;
	BOOST_PP_REPEAT(N,SCOPE_GUARD_DECLARE_PARAM, ~)

public:
	BOOST_PP_CAT(scope_guard_impl, N)(const	Fun& fun BOOST_PP_REPEAT(N,SCOPE_GUARD_CONST_PARAM_LIST, ~))
		:	fun_(fun) BOOST_PP_REPEAT(N,SCOPE_GUARD_PARAM_INIT_LIST, ~)
	{
	}

	// Method that does the action immediately.  Does not swallow exceptions.
	virtual void doit() const {
		if(!dismissed_){
			// dismiss first, so it doesn't get called more than once
			dismissed_ = true;
			fun_(BOOST_PP_REPEAT(N,SCOPE_GUARD_CALL_PARAM_LIST, ~));
		} 
	}

	~BOOST_PP_CAT(scope_guard_impl,	N)(){
		if(!dismissed_){
			try{
				fun_(BOOST_PP_REPEAT(N,SCOPE_GUARD_CALL_PARAM_LIST, ~));
			}catch(...){}
		} 
	}
};

// make_guard Helper Function
template<typename Fun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>
BOOST_PP_CAT(scope_guard_impl, N)<Fun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, P)> make_guard(const Fun& fun BOOST_PP_REPEAT(N,SCOPE_GUARD_CONST_PARAM_LIST, ~))
{
	return BOOST_PP_CAT(scope_guard_impl, N)<Fun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, P)>(fun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, p));
}


/*	----------------------------------------------------------------------	*\
	scope_guard

	Type:		Member function
	# Params:	N
\*	----------------------------------------------------------------------	*/
template<class Obj, typename MemFun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class P)>
class BOOST_PP_CAT(obj_scope_guard_impl, N)	: public scope_guard_impl_base
{
	Obj& obj_;
	MemFun fun_;
	BOOST_PP_REPEAT(N,SCOPE_GUARD_DECLARE_PARAM, ~)

public:
	BOOST_PP_CAT(obj_scope_guard_impl, N)(Obj& obj, const	MemFun& fun BOOST_PP_REPEAT(N,SCOPE_GUARD_CONST_PARAM_LIST, ~))
		:	obj_(obj),fun_(fun) BOOST_PP_REPEAT(N,SCOPE_GUARD_PARAM_INIT_LIST, ~)
	{
	}

	// Method that does the action immediately.  Does not swallow exceptions.
	virtual void doit() const {
		if(!dismissed_){
			// dismiss first, so it doesn't get called more than once
			dismissed_ = true;
			(obj_.*fun_)(BOOST_PP_REPEAT(N,SCOPE_GUARD_CALL_PARAM_LIST, ~));
		} 
	}

	~BOOST_PP_CAT(obj_scope_guard_impl,	N)(){
		if(!dismissed_){
			try{
				(obj_.*fun_)(BOOST_PP_REPEAT(N,SCOPE_GUARD_CALL_PARAM_LIST, ~));
			}catch(...){}
		} 
	}
};

// make_guard Helper Function
template<class Obj, typename MemFun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename P)>
BOOST_PP_CAT(obj_scope_guard_impl, N)<Obj, MemFun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, P)> make_guard(Obj& obj,const MemFun& fun BOOST_PP_REPEAT(N,SCOPE_GUARD_CONST_PARAM_LIST, ~))
{
	return BOOST_PP_CAT(obj_scope_guard_impl, N)<Obj, MemFun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, P)>(obj,fun BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, p));
}
#undef N