
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: debug.jl,v 1.1 1995/11/23 04:30:30 jaw Exp jaw $

;; debugging stuff

(defvar show-trace-on-error #f
  "should we get a stack trace on error?")

(defvar debug-on-error #f
  "should we enter the debugger on error?")

(defvar *error-object* ()
  "the object which caused the error")

(defun dbg:truncate (obj len)
  (let ((str (strcpy ""))
	(sp  (open:string str)))
    (display obj sp)
    (if (> (length str) len)
	  (set! str (strcat (substr str 0 len) "...")))
    str))

{defun show-trace (&optional how)
  "(show-trace) display a backtrace of calls made
if given an argument, will be more verbose"
  (let ((bt (backtrace)))
    (while (nnullp bt)
      (let ((fn (cond
		((nullp (caar bt))
		 (display "signal #")
		 (nth (car bt) 2))
		((functionp (caar bt))
		 "anonymous function")
		((macrop (caar bt))
		 "anonymous macro")
		(#t
		 (caar bt)))))
	(if (= 1 (nth (car bt) 3))
	    (display "* "))
	(if (boundp how)
	    (display (dbg:truncate (cons fn (nth (car bt) 2)) 70))
	  (display fn)))
      (newline)
	   (set! bt (cdr bt))))}

{defun debugger-called-from ()
  "(debugger-called-from) who called the debugger?"
  (let ((bt (backtrace)))
    (while (and (nnullp bt) (not (eq (caar bt) 'debugger)))
      (set! bt (cdr bt)))
    (set! bt (cdr bt))
    (let ((fr (car bt)))
      (cons (nth fr 0) (nth fr 2))))}

{defun error (fnc obj huh)
  "(error fnc obj descr) signal an error"
  (throw 'error  obj)
  (set! *error-object* obj)
  (print "ERROR: " fnc ": in file \"" *current-file*   "\" near line " .lineno ": ``" obj "'' -- " huh)
  (newline)
  (if show-trace-on-error
      (show-trace #t))
  (if debug-on-error
      (debugger 'error ())
    (throw 'repl obj))}

(define error-handler error
  "called by the internal error handling code to handle errors")

;; when=0 => before call; when=1 => after call
{defun debugger (when retval)
  "(debugger) entry point into the debugging routines from the internals
the following commands are available at the [debugger] prompt:
    c   continue execution
    q   quit to the repl
    s   step
    b n set breakpoint at level n
    u   clear debug at end of call flag
    r o set the return value for the function
    p e print the value of the expression
    ?   does not print this message"
  (newline)
  ;; (display "Entering debugger")(newline)
  (print ">> " (dbg:truncate (debugger-called-from) 40) " <<" )
  (if (eq when 1)
      (print "\t=> " retval))
  (newline)
  (while
	(debugger-cmd))
  retval}

{defun debugger-cmd ()
  (display "[debugger] ")
  (let ((rspns (getc)))

    (while (case rspns
		 ;; eat white space
		 ((?\s ?\t ?\n ?\r)
		  (set! rspns (getc)))
		 (#t #f)))
    (case rspns
	  (?c #f)                                     ; continue
	  (?s (if (not (eq when 1))                   ; step
		  (set! .debug-on-next-call #t))
	      #f)
	  (?n (set! .debug-on-next-call #f)           ; next
	      #f)  
	  (?t (show-trace)                            ; trace
	      #t)
	  (?q (set! .already-debugging #f)            ; quit
	      (throw 'repl)
	      #f)
	  (?u (set-debug-backtrace -1)                ; undo dbg on end of fnc flag
	      #t)
	  (?b (if (catch 'error                       ; set dbg on end of fnc flag
		    (set-debug-backtrace (eval (read)))
		    (show-trace)
		    #f)
		  (display "Error\n"))
	      #t)
	  (?r (if (eq when 0)                         ; set retval
		  (display "r command is valid only while _exiting_ a function")
		(if (catch 'error
		      (set! retval (eval (read)))
		      #f)
		    (display "Error\n"))) 
	      #t)
	  (?p (if (catch 'error                           ; print arb expr
		    (display (eval (read)))
		    (newline)
		    #f)
		  (display "Error\n"))
	      #t)
	  (?? (display (docstr debugger))             ; help me
	      (newline)
	      #t)
	  (#t (display "Huh???\n")
	      #t)))}


;; define debugging sections and fields
;; from include/debugging.h
(define SECTION_MRI     0)
(define SECTION_LISP    1)
(define SECTION_INIT    2)
(define SECTION_SNAKE   3)
(define SECTION_SEGM    4)
(define SECTION_WINSYS  5)

(define DBG_VERBOSE     0)
(define DBG_ECHO        1)
(define DBG_RESULT      2)
(define DBG_SAVE        3)
(define DBG_THINK       4)
(define DBG_WORK        5)
(define DBG_SAVE_X      6)
(define DBG_STATS       7)

(if (not (definedp 'set-debug-flag!))
    (defun set-debug-flag! (sect fld)
      ()))

(if (not (definedp 'debug-flag))
    (defun debug-flag (sect fld)
      #f))


