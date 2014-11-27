
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: lib.jl,v 1.1 1995/11/23 04:30:31 jaw Exp jaw $

(defvar *unintered-symbol-maker-counter* 0)

(defun cddr   (x) "(cddr x) ..."   (#.cdr (#.cdr x)))
(defun caar   (x) "(caar x) ..."   (#.car (#.car x)))
(defun cdar   (x) "(cdar x) ..."   (#.cdr (#.car x)))
(defun cadr   (x) "(cadr x) ..."   (#.car (#.cdr x)))
(defun cdddr  (x) "(cdddr x) ..."  (#.cdr (#.cddr x)))
(defun caddr  (x) "(caddr x) ..."  (#.car (#.cddr x)))
(defun cadddr (x) "(cadddr x) ..." (#.car (#.cdddr x)))

{defun version ()
  "(version) What version are we using?"
  (display .version)
  (newline)}

{defun featurep (f)
  "(featurep feature) is feature available?"
  (and (memq f *features*) #t)}

{defmac docstr (s)
  "(docstr symbol) retreive the documentation from a symbol"
  (let* ((pl (if (definedp s)
		 (get-props s)))
	 (ds (if (listp pl)
		 (assq '.docstring pl))))
    (if (consp ds)
	(cdr ds)
      "No documentation available"))}


{defun cat-file (file)
  "(cat-file file) cat a file to stdout"
  (system (strcat "cat " (expand-filename file)))}

{defun make-range (lo hi)
  "(make-range lo hi) return a list of numbers from lo to hi (inclusive)"
  (cond
   ((>= lo hi)
    (list hi))
   (#t
    (cons lo (make-range (+ lo 1) hi))))}

;; extended define
{defmac xdefine (name val pred prepr &optional doc)
  "(xdefine name value predicate preproc [docstring])
defines name as val and adds a few properties to the alist, for type safety
[see also: sets!]"

  (if (definedp name)
      `(print ',name " already defined\n")
    `(progn
       (define ,name ,val ,doc)
       (set-props! ,name (acons 'predicate ,pred
				(acons 'preproc ,prepr
				       (get-props ',name))))
       ()))}


{defmac sets! (name val)
  "(sets! name val) sets name to val, possibly with some type checking..."
  (if (ndefinedp name)
      `(error "sets!" ,name "not defined")
    (let* ((value (eval val))
	   (pl (get-props name))
	   (okp (assq 'predicate pl))
	   (pp  (assq 'preproc   pl))
	   (v (if (and pp (cdr pp) (nnullp (cdr pp)))
		  ((cdr pp) value)
		value)))
      (if (or (not (and okp (cdr okp) (nnullp okp))) ((cdr okp) v))
	  `(set! ,name ',v)
	`(error "sets!" ,val "bad value"))))}


; SAP p. 97
{defun reverse (l)
  "(reverse list) reverse a list"
  (if (nconsp l)
      l
    (append (reverse (cdr l)) (list (car l))))}

{defun 1+ (i)
  "(1+ i) return (+ 1 i)"
  (+ 1 i)}

{defun 1- (i)
  "(1- i) return (+ -1 i)"
  (+ -1 i)}

{defmac ++ (i)
  "(++ i) increment a number"
  `(progn
    (set! ,i (1+ ,i))
    ,i)}

{defmac -- (i)
  "(-- i) decrement a number"
  `(progn
     (set! ,i (1- ,i))
     ,i)}

{defun != (x y)
  "(!= a b) are they different?
[see also: = < > <= >=]"
  (not (= x y))}

{defun string->number (str (%input-radix% 10))
  "(string->number string [radix]) convert string to a number
[see also: number->string]"
    (read (open:string str))}

{defun number->string (n (%output-radix% 10))
  "(number->string number [radix]) convert number to a string
[see also: string->number]"
  (let ((str (strcpy ""))
	(sp (open:string str)))
    (write n sp)
    str)}

{defun getline (&optional (port *stdin_port*))
  "(getline [port]) read in a line [from port] will return () on eof"
  (let ((s (strcpy ""))
	(e (eof-object))
	(c ()))
    (if (catch 'eof
	  (while (not (or (eq c ?\n) (eq c e)))
	    (set! c (getc port))
	    (strappend! s c)))     ; strings magically grow
	(progn
	  (throw 'eof)
	  ())
      s))}


(defun print-with-port (port &rest argl)
  "(print-with-port port args...) print the args on specified port"
  (while (nnullp argl)
    (display (car argl) port)
    (set! argl (cdr argl))))
(defun print-stderr argl
  "(print-stderr port args...) print the args on stderr"
  (apply print-with-port *stderr_port* argl))


(defun nop argl
  "(nop) does nothing")

{defun acons (a b c)
  "(acons key value alist) add key/value pair to an alist"
  (cons (cons a b) c)}

{defun cons2 (a b c)
  "(cons2 a b c) add a and b to front of list c"
  (cons a (cons b c))}


{defun memoize (fnc)
  (eval
   `(lambda (x)
      (let* ((memo '( () . () ) )
	     (r (assv x memo)))
	(if r
	    (cdr r)
	  (let ((y (,fnc x)))
	    ; (print "calculating\n")
	    (append! memo (cons (cons x y) ()))
	    y)))))}


{defun die (&optional (message #f))
  "(die [message]) print out the message and exit
[see also: quit _quit]"
  (if message
      (print-stderr message ?\n))
  (quit 1)}

{defun output-of-shell-command->string (cmd)
  "(output-of-shell-command->string command) return the output of running the command
much like using `command` in the shell"
  (let ((str (strcpy ""))
	(ln #t)
	(fp  (open:read (strcat "|" cmd))))
    (catch 'eof
      (while (nnullp (set! ln (getline fp)))
	(strappend! str ln)))
    str)}

{defun unintered-symbol ()
  "(unintered-symbol) returns a unique unintered symbol"
  (++ *unintered-symbol-maker-counter*)
  (string->symbol (strcat "#<G-"
			  (number->string *unintered-symbol-maker-counter* 36)
			  ">"))}

(define gensym unintered-symbol)



