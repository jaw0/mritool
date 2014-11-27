
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: init.cf.jl,v 1.1 1995/11/23 04:30:34 jaw Exp jaw $

(set! .hash_table_size 7)    ; most often a small table will suffice
			     ; this was determined empirically
(set! .box_size 1024)        ; allocate cells at a time
(set! .gc_thresh (* .box_size 50))     ; keep threshhold high


;;; yes, defun/defmac could possibly have been combound, but
;;; it becomes an unreadable mess of `,`,',`',`',,`,```,,',,,

;;; for space savings the docstr and defined-in-file props can be removed

{define defun
  "(defun name args [docstr] body...) Define a Function"
  (macro (func argl &rest args)
    (let* ((docstr (car args))
	   (body (cdr args)))
      (if (stringp docstr)
	  #t
	(set! docstr "Not Documented")
	(set! body args))
      `(progn
	 (define ,func ,docstr
	   ,(cons lambda (cons argl body)))
	 (set-props! ,func (cons
			    (cons 'defined-in-file ,*current-file*)
			    (get-props ',func)))
	 ,func			; retval
	 )))}

{define defmac
  "(defmac name args [docstr] body...) Define a macro"
  (macro (func argl &rest args)
    (let* ((docstr (car args))
	   (body (cdr args)))
      (if (stringp docstr)
	  #t
	(set! docstr "Not Documented")
	(set! body args))
      `(progn
	 (define ,func ,docstr
	   ,(cons macro (cons argl body)))
	 (set-props! ,func (cons
			    (cons 'defined-in-file ,*current-file*)
			    (get-props ',func)))
	 ,func			; retval
	 )))}

{defmac defvar (sym val &optional doc)
  "(defvar var initvalue [docstring]) defines var as initvalue only if var is undefined"
  (if (definedp sym)
      `()
    `(define ,sym ,val ,doc))}

{defun print argl
  "(print args...) print the args on stdout"
  (while (not (nullp argl))
    (display (car argl))
    (set! argl (cdr argl)))}

{defun newline (&optional port)
  "(newline [port]) output a newline [to a specified port]"
  (display ?\n port)}

(defvar load-path
  "load-path  list of directories to search for lisp files"
  (list
   "%LOCALLISP%"
   "%LISPDIR%"
   "%ETCDIR%"         ; start grasping at straws
   "%SRCDIR%/lisp"
   "%SRCDIR%/jlisp"
   "%SRCDIR%/lib" ))

(defvar load-extensions
  "load-extensions  list of extensions to try for lisp files"
  (list
   ".jl" 
   ".jlisp"))

(defvar *load:echo*    #f)  ; echo filenames as loaded
(defvar *load:verbose* #f)  ; echo each exp of the file as it is read

(defvar *builtin-load* load)
(defvar *current-file* "init")

;;; redefine load, the builtin is just a minimal stub
;;; this is a macro, as it must execute in the current env frame
{defmac load (file)
  "(load file) load a file"
  (let* ((efn (eval file))
	 (fp (cond
	      ((inputportp efn) efn)
	      ((stringp efn) (let ((foo ())
				   bar
				   baz
				   (l (append '(()) load-path))           ; try as given first
				   (e ()))
			       ;; search for the file
			       (while (and (nullp foo) (not (nullp l)))
				 (set! bar (if (stringp (car l))
					       (strcat (car l) "/" efn)
					     efn))
				 (set! e (append '(()) load-extensions))  ; as given first
				 (while (and (nullp foo) (not (nullp e)))
				   (set! baz (car e))
				   ;; saved in file so we can access it later if need be
				   (set! file (if (stringp baz)	
						  (strcat bar baz)
						bar))
				   (set! foo (open:read file))
				   (set! e (cdr e)))
				 (set! l (cdr l)))
			       foo))
	      (#t
	       (funcall error "load" efn "WTA: filename or port p")))))
    (if (nullp fp)
	(funcall error "load" efn "Could not open"))
    (if (or *load:echo*
	    (and (definedp 'mritool) (debug-flag 1 1)))  ;  lisp, echo
	(progn (display "Loading: ") (display file) (display ?\n)))
    ;; the following will be executed in the calling env
    `(unwind-protect
	 (progn
	   (set! .lineno 1)
	   (set! *current-file* ,file)
	   (if (catch 'error              ; catch any errors, handle below
		 (catch 'eof
		   (if (or *load:verbose*
			   (and (definedp 'mritool) (debug-flag 1 0)))  ; lisp, verbose
		       (while #t
			 (eval (let ((foo (read ,fp)))
				 (display foo) (display ?\n)
				 foo)))
		     (while #t
		       (eval (read ,fp)))))
		 #f)
	       ;; handle errors
	       (print "\nERROR while loading \"" ,file "\" near line " .lineno ?\n))
	   (close ,fp))
       (set! *current-file* ,*current-file*) ; restore filename
       (set! .lineno ,.lineno)))}            ; restore lineno

;;; XXX ought test for req. features before loading...
;; load more
(load "debug.jl")
(load "autoload.jl")
(load "lib.jl")
(load "math.jl")
(load "pred.jl")
(load "cmdline.jl")
(load "repl.jl")
(load "signal.jl")
(load "unistd.jl")
; uncomment the following 2 for more scheme-itivity
(load "slib.jl")
(load "r4rs.jl")
; and one for some CL-isms
(load "cl.jl")
; the jlisp object system
(load "objsys.jl")

(autoload expand-filename "expand.jl"   "expand ~ in filenames")
(autoload roman           "roman.jl"    "return the roman numeral represenation of the number")
(autoload english         "roman.jl"    "return the english represenation of the number")
(autoload bind            "bind.jl"     "bind function calls")
(autoload format          "format.jl"   "formatted output")
(autoload appropos        "all-syms.jl" "what function? by keyword")
(autoload time            "time.jl"     "how long does it take?")
(autoload abusage         "usage.jl"    "report on system use")
(autoload defclass        "objsys.jl"   "define a class")

(if (definedp 'mritool)
    (progn
      (load "mritool.jl")
      (define mri:background #t)    ; used internally by initliaztion in c code
      (define mri:windows    #t)    ; ditto
      (define mri:winsys     'x)))  ; what window system are we using


