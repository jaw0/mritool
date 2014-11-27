
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: repl.jl,v 1.1 1995/11/23 04:30:32 jaw Exp jaw $

(defun repl ()
  "(repl) read eval print loop "
  ;; catch eof and exit
  (set! .lineno 1)
  (set! *current-file* "user input")
  (while #t
    (if (catch 'repl
	  ;; display a prompt...
	  (prompt)
	  (display (eval (read)))
	  #|
	    (display (eval (let ((val (read)))
			     (if (memq val '(quit exit bye-bye))
				 ;; clueless user
				 (quit)
			       val))))
	  |#
	  (display ?\n)
	  #f)
	(display "repl: Error detected\n"))))

(defun prompt ()
  "(prompt) displays the repl prompt"
  (display "> "))

(define %repl% '(catch 'eof (repl)))

