
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: objsys.jl,v 1.1 1995/11/23 04:30:39 jaw Exp jaw $

;;; jlisp object system
;;; modelled roughly after C++

;;;; ToDo copy-class

#|
element attributes : static      -- only one per class
                   : read-only   -- may only be set in the ctor
                   : initial     -- (initial value) default ctor uses this to set the initial value
                   : (RSN: private/protected ...)
method attributes  : virtual     -- func is virtual 
                   : after       -- call func after calling base class func
                   : before      -- call func before calling base class func (both or neither of these can be set)
                   : all-bases   -- call func in all base classes, not just first one found
                   : (RSN: private/protected  ...)
class attributes   : all-bases   -- call func in all base classes, not just first one found
|#

#|
Impl. Details (subject to change at whim):

object consists of vector:
    #(unique-tag vptr base-vectors... elements...)

the prop-list on the unique-tag is an alist containing:
    name        the class name
    tag         the unique tag
    length      length of the vector
    attribs     list of attributes
    nbase       number of base classes
    nelem       number of elements
    statvec     vector of values for static elements
    base        list describing bases, consisting of:
             (name-of-base unique-tag-of-base offset-in-vector)

    elems       list describing the elements, consisting of:
             (name-of-element offset-in-vector attrib-list)

    methods     list describing methods, consisting of:
             (name-of-method virtual-p function attrib-list)

|#

(defvar *class:htable* (makevector 1021))
(defvar *class:pred-suffix* "-p"
  "*class:pred-suffix* prefered suffix for predicate function, typically -p or ?")

(defmac defclass (pname (eleml ()) (methl ()) &rest attrl)
  "(defclass name (elems...) (meths...) attributes...) define a class
name  : name | (name base...)
elems : elemnt-name | (element-name attributes...)
meths : (method-name defn attributes...)"

  (if (nconsp eleml)
      (progn
	(set! attrl (cons eleml attrl))
	(set! eleml ())))
  (if (nconsp methl)
      (progn
	(set! attrl (cons methl attrl))
	(set! methl ())))
  
  (let* ((name  (if (consp pname) (car pname) pname));name of class
	 (bases ())			             ;base classes
	 (nbase 0)
	 (elems ())			             ;element list
	 (nelem 0)
	 (statics ())				     ;static elements
	 (nstatic 0)
	 (methods ())				     ;method list
	 (nmeth 0)
	 (tag (inter (gensym)))
	 (alist ()) 
	 (predname (string->symbol (strcat (symbol->string name) *class:pred-suffix*)))
	 fnew fpred hv
	 slen btag)

    ;; a derived class?
    (if (consp pname)
	;; go thru base classes
	(dolist (b (cdr pname))
	  ;; find base class defn
	  (set! btag (cdr (assq 'tag
				(assq b (nth *class:htable*
					     (% (hashv b)
						(length *class:htable*)))))))
	  ;; (set! btag (cdr (assq 'tag (assq b *class:alist*))))
	  (if (not btag)
	      (error "defclass" b "is not a known class name"))
	  (set! bases (cons
		       (list b btag (+ 2 nbase))
		       bases))
	  (++ nbase)))

    (dolist (mth methl)
      ;; go thru methods

      (if (nconsp mth)
	  (error "defclass" mth "invalid method spec"))

      (let* ((mname (car mth))
	     (defn  #f)
	     (attr  ())
	     (el (cdr mth))
	     (e  (car el))
	     virt-p )

	(while (and (nnullp e) (nnullp (cdr el)))
	  ;; split into defn and attrs
	  (cond
	   ((symbolp e)
	    (set! attr (cons e attr)))
	   (#t
	    (error "defclass" e "invalid method attribute")))
	  (set! el (cdr el))
	  (set! e  (car el)))
	(set! defn (eval e))
	
	(set! virt-p (and (memq 'virtual attr) #t))
	(set! methods (cons
		       (list mname virt-p defn attr)
		       methods))
	(++ nmeth)))
    ;; RSN virtual-ness ought be inheirited from base class method ...

    
    (dolist (elm eleml)
      ;; go thru elements
      (cond

       ((symbolp elm)
	;; simple element
	(set! elems (cons
		     (list elm (+ nelem nbase 2) ())
		     elems))
	(++ nelem))

       ((consp elm)
	;; complex element
	(if (memq 'static (cdr elm))
	    ;; static element
	    (progn
	      (set! statics (cons
			     (list (car elm) nstatic (cdr elm)) ;name value attribs
			     statics))
	      (++ nstatic))
	  ;; non static complex element
	  (set! elems (cons
		       (list (car elm) (+ nelem nbase 2) (cdr elm))
		       elems))
	  (++ nelem)))
      (#t
       (error "defclass" elm "invalid element spec"))))
    
    (set! slen (+ nelem nbase 2))

    ;; create some default methods

    ;; function new
    (set! fnew (eval `(lambda (&optional derivedp)
		  (let* ((s (makestruct ,slen))
			 (pl (get-props ',tag))
			 (ct (assq 'ctor (cdr (assq 'methods pl))))
			 (bl (cdr (assq 'base pl)))
			 bt)
		    (if (nboundp derivedp) (set! derivedp s))
		    (set-nth! s 0 ',tag)
		    (set-nth! s 1 #f)	;no virtual fncs until after constructed
		    (dolist (b bl)
		      ;; instantiate bases
		      (set! bt (get-props (cadr b)))
		      (set-nth! s (caddr b)
				;; find and call base class fnc new
				((caddr (assq 'new (cdr (assq 'methods bt))))
				 derivedp)))
		    ;; call ctor
		    (if ct ((caddr ct) s))
		    (set-nth! s 1 derivedp)
		    s))))
    
    (set! fpred (eval `(lambda (obj)
			(and (structp obj) (eq (nth obj 0) ',tag)))))

    ;; attach default methods
    (set! methods (append
		   methods
		   (list
		    (list 'new   #f fnew ())  ; can not be virtual
		    (list 'pred  #t fpred ()) ; if not virtual, would always be #t
		    (list 'print #f class:print '(before all-bases))
		    (list 'ctor  #f class:ctor ())
		    (list 'get   #f class:get ())
		    (list 'set   #f class:set ()))))
    ;; attach conversions to base classes (named after that class)
    (dolist (b bases)
      (set! methods (append
		     methods
		     (list
		      (list (nth b 0) #f (eval `(lambda (obj)
					    (nth obj ,(nth b 2)))) ())))))

    ;; attach data to tag
    (set! alist (append (list
			 (cons 'name    name)
			 (cons 'tag     tag)
			 (cons 'attribs attrl)
			 (cons 'length  slen)
			 (cons 'nbase   nbase)
			 (cons 'base    bases)
			 (cons 'nelem   nelem)
			 (cons 'elems   elems)
			 (cons 'statics statics)
			 (cons 'statvec (makevector nstatic))
			 (cons 'methods methods))
			alist))

    (set! hv (% (hashv name) (length *class:htable*)))
    (set-nth! *class:htable* hv (acons name alist (nth *class:htable* hv)))
    ;; (set! *class:alist* (acons name alist *class:alist*))
    (set-props-nq! tag  alist)

    ;; (print "pl: " (get-props tag) ?\n)

    `(progn
       (define ,name ,fnew)
       (define ,predname ,fpred)
    )))

;;; locate element in object
;;; returns (offset vector attriblist)
;;; or #f
(defun class:locate-elem (this elem)
  (let* ((pl    (get-props (nth this 0)))
	 (eleml (cdr (assq 'elems pl)))
	 (basel (cdr (assq 'base pl)))
	 (statl (cdr (assq 'statics pl)))
	 v g)
    (cond
     ((set! v (assq elem statl))
      ;; it is a static
      (list (nth v 1) (cdr (assq 'statvec pl)) (nth v 2)))
     ((set! v (assq elem eleml))
      ;; it is an element of this
      (list (nth v 1) this (nth v 2)))
     (#t
      ;; check base classes
      (set! v (car basel))
      (while (and (nnullp v) (not
			      (set! g (class:locate-elem (nth this (nth v 2)) elem))))
	(set! basel (cdr basel))
	(set! v (car basel)))
      g))))

;;; default accessor function
(defun class:get (this elem)
  (let* ((loc (class:locate-elem this elem)))

    (if loc
	(nth (nth loc 1) (nth loc 0))
      (error "class:get" elem "cannot find such an element"))))

;;; default settor function
(defun class:set (this &rest argl)
  (while (nnullp argl)
    (let ((elem (car argl))
	  (value (cadr argl)))
      (let ((loc (class:locate-elem this elem)))
	(if loc
	    (if (and (nth this 1) (memq 'read-only (nth loc 2)))
		;; can set ro elem in ctor only
		(error "class:get" elem "read-only element")
	      (set-nth! (nth loc 1) (nth loc 0) value))
	  (error "class:get" elem "cannot find such an element"))))
    (set! argl (cddr argl))))

;;; default print
(defun class:print (this &optional port)
  (let ((pl (get-props (nth this 0)))
	(eleml (cdr (assq 'elems pl))))
    (dolist (e eleml)
      (display (nth e 0) port)
      (display ": " port)
      (display (nth this (nth e 1)) port)
      (display "  " port))))
	
;;; default ctor
;;; set up initial values
(defun class:ctor (this)
  (let ((pl (get-props (nth this 0)))
	(eleml (cdr (assq 'elems pl))))

    (dolist (e eleml)
      (let* ((offset (nth e 1))
	     (attr   (nth e 2))
	     (init   (cadr (assq 'initial attr)))) ;should this get evalled ?
	(set-nth! this offset init)))))
	
;;; method displatch
(defun method (this mname &rest argl)
  "(method object method-name args...) call a class method"
  (if (structp this)
      (let ((cdm:got #f))
	(class:do-method #t this mname argl))
      (apply (string->symbol (strcat (typeof this) ?: (symbol->string mname))) this argl)))

(defun class:do-method (do-vfp this mname argl)

  (let* ((mtl  (assq mname (cdr (assq 'methods (get-props (nth this 0))))))
	 (vfp  (and do-vfp (cadr mtl)))
	 (that (if vfp (nth this 1) this))
	 (thaa (get-props (nth that 0)))
	 (fnl  (assq mname (cdr (assq 'methods thaa))))
	 (fnc  (if fnl (nth fnl 2) #f))
	 (fna  (if fnl (nth fnl 3) ()))

	 ;; how, when, and which bases methods to do
	 (before (and fnc (memq 'before fna)))
	 (after  (and fnc (memq 'after  fna)))
	 (do-bases (or before after (not fnc)))
	 (allbasep (memq 'all-bases (if fnc
					   fna
					 (cdr (memq 'attribs thaa)))))
	 (bl (cdr (assq 'base thaa)))
	 (b (car bl))
	 result)

    ;; default to before if nothing specified ?
    (if (and (not before) (not after) fnc)
	(set! before #t))

    ;; should before AND after be disallowed?
    ;; (if (and before after)
    ;;     (error "method" mname "before and after ?"))
    
    (if before
	(set! result (apply fnc that argl)))

    ;; call base methods
    (if do-bases 
	(dolist (b bl)
	  (if (or allbasep (not cdm:got))
	      (set! result (class:do-method #f (nth that (nth b 2)) mname argl)))))
    
    (if after
	(set! result (apply fnc that argl)))
    (if fnc
	(set! cdm:got #t))
    result))

(defun add-method (clname mthname &rest argl)
  "(add-method classname methodname attributes... func) add a method to a class"
  (let* ((pl (cdr (assq clname (nth *class:htable*
			       (% (hashv clname)
				  (length *class:htable*))))))
	 (mtha (assq 'methods pl))
	 (meths (cdr mtha))
	 (defn #f)
	 (attr ())
	 (el argl)
	 (e (car el))
	 virt-p)
    
    (while (and (nnullp e) (nnullp (cdr el)))
	  ;; split into defn and attrs
	  (cond
	   ((symbolp e)
	    (set! attr (cons e attr)))
	   (#t
	    (error "add-method" e "invalid method attribute")))
	  (set! el (cdr el))
	  (set! e  (car el)))
	(set! defn (eval e))
	
	(set! virt-p (and (memq 'virtual attr) #t))
	(set-cdr! mtha (cons
		       (list mthname virt-p defn attr)
		       meths))))


;;;=======================================================================================
;;; test classes
#|
(defclass foo
  (x
   (y read-only (initial "Why?"))
   z
   (q static))
  ((proc (lambda (x) (display "foo::proc\n")))
   (func (lambda (x) (display "foo::func\n")))
   (vfnc virtual (lambda (x) (display "foo::vfnc\n")))))

(defclass (bar foo)
  (a
   (b read-only)
   (c ugly))
  ((proc (lambda (x) (display "bar::proc\n")))
   (vfnc virtual (lambda (x) (display "bar::vfnc\n")))
   (ctor (lambda (this)
	   (method this 'set 'a 0)
	   (method this 'set 'b "Glark!")
	   (method this 'set 'c *)))))


(defclass Person
  ((name (initial "John Doe"))
   (ssn  (initial "000-00-0000")))
  ((print virtual (lambda (this &optional port)
		    (display (method this 'get 'name) port)))))

(defclass (Employee Person)
  ((wage  (initial 3.75))
   (hours (initial 0))
   (dept  (initial "MailRoom")))
  ((print virtual after (lambda (this &optional port)
			  (display " (" port)
			  (display (method this 'get 'dept) port)
			  (display ")" port )))))

(defclass (Manager Employee)
  ((peons ()))
  ((print virtual before (lambda (this &optional port)
			   (display "Mngr. " port)))))


|#
