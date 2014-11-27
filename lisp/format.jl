 
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"
 
;;;; $Id: format.jl,v 1.1 1995/11/23 04:30:32 jaw Exp jaw $
 
;;; an inplementation of format for jlisp
;;; the format spec directives are as described in the slib format info page
;;; I have not verified if these are the same as CL

;;; floating pt. (~f ~e ~g) is not yet impl.
;;; the docstring on format:spec details it all...


(defun format:char-is-white (c)
  (case c
	((?\s ?\t ?\r ?\n ?\f) #t)
	(#t #f)))

(defun format:->string (fnc obj)
  (let ((str (strcpy ""))
	(fp  (open:string str)))
    (fnc obj fp)
    str))

(defun format:format-string (obj fnc mincol padchar ovchar gravity)
  (let* ((str (format:->string fnc obj))
	 (leng (length str))
	 (width (if (nnullp mincol) (->int (abs mincol)) 0))
	 (maxcolp (and (nnullp mincol) (< mincol 0))))
    (cond
     ((= leng width)
      ;; is proper size -- nop
      )
     ((> leng width)
      ;; is long -- chop or leave ?
      (if maxcolp
	  (progn
	    (set! str (substr str 0 ( - width 1)))
	    (set! str (case gravity
			    ((r) (strcat ovchar str))
			    ((l) (strcat str ovchar))
			    (#t  (strcat str ovchar)))))))
      
     ((< leng width)
      ;; is short -- pad?
      (set! str (case gravity
		      ((r) (strcat (makestr (- width leng) padchar) str))
		      ((l) (strcat str (makestr (- width leng) padchar)))
		      ((c) (let* ((rpl (->int (/ (- width leng) 2)))
				  (lpl (- width rpl)))
			     (strcat (makestr lpl padchar) str (makestr rpl padchar))))))))
    (display str format-port)))

(defun format:format-string-hp (obj fnc)
  (let* ((gravity (if got-atsign 'r 'l))
	 (mincol  (if (nnullp params)
		      (let ((mincol (car params)))
			(set! params (cdr params))
			mincol)
		    ()))
	 (padchar (if (nnullp params)
		      (let ((padchar (int->char (car params))))
			(set! params (cdr params))
			padchar)
		    ?\s))
	 (ovchar (if (nnullp params)
		      (let ((ov (int->char (car params))))
			(set! params (cdr params))
			ov)
		   (case gravity
			 ((r) ?<)
			 ((l) ?>)
			 (#t  ?*)))))
    (format:format-string obj fnc mincol padchar ovchar gravity)))

(defun format:do-number (n base prefix)
  (let* ((mincol  (if (nnullp params)
		      (let ((mincol (car params)))
			(set! params (cdr params))
			mincol)
		    ()))
	 (padchar (if (nnullp params)
		      (let ((padchar (int->char (car params))))
			(set! params (cdr params))
			padchar)
		    ?\s))
	 (ovchar (if (nnullp params)
		     (let ((ov (int->char (car params))))
		       (set! params (cdr params))
			ov)
		   ?*))
	 (commachar (if (nnullp params)
		     (let ((cc (int->char (car params))))
		       (set! params (cdr params))
			cc)
		   ?,))
	 (commawidth (if (nnullp params)
		     (let ((cw (car params)))
		       (set! params (cdr params))
			cw)
		   3))
	 (pnstr (number->string n base))
	 (nstr  (if (not got-colon) pnstr
		  ;; insert commas
		  (let* ((sl (length pnstr))
			 (ss (if (zerop (% sl commawidth)) commawidth (% sl commawidth)))
			 (str (substr pnstr 0 ss)))
		    (while (< ss sl)
		      (strappend! str commachar (substr pnstr ss commawidth))
		      (set! ss (+ ss commawidth)))
		    str)))
	 (str (if got-atsign (strcat prefix nstr) nstr)))
    (format:format-string str display mincol padchar ovchar 'r)))

    
(defun format:ctoi (c)
  (- (char->int c) (char->int ?0)))

(defun format:display-ntimes (c n)
  (display (makestr (if (nullp n) 1 n) c) format-port))

(defun format:parse ()
  ;; does not catch most illegal specs
  (let ((n ())
	how
	(sgn #f))
    (while (case (nth fspec spec-index)
		 ((?1 ?2 ?3 ?4 ?5 ?6 ?7 ?8 ?9 ?0)
		  ;; handle number
		  (set! n (if (nullp n)
			      (format:ctoi (nth fspec spec-index))
			    (+ (* 10 n) (format:ctoi (nth fspec spec-index))))))
		 ((?-) (set! sgn #t))
		 ((?,)
		  (set! params (append! params (list (if sgn (* n -1) n))))
		  (set! sgn #f)
		  (set! n ()))
		 ((?:) (set! got-colon  #t))
		 ((?@) (set! got-atsign #t))
		 ((?#)
		  ;; param from number of args left
		  (set! n (length args)))
		 ((?V ?v)
		  ;; param from arg list
		  (set! n curr-arg)
		  (set! last-arg curr-arg)
		  (set! args (cdr args))
		  (set! curr-arg (car args)))
		 (#t #f))
      (++ spec-index))
    (if (nnullp n)
	(set! params (append! params (list (if sgn (* n -1) n)))))
    (set! how (nth fspec spec-index))
    (case how
	  ;; handle perl-isms
	  ((?< ?> ?.)
	   (set! n 1)
	   (while (or (eq ?< how) (eq ?> how) (eq ?. how))
	     (++ n)
	     (++ spec-index)
	     (set! how (nth fspec spec-index)))
	   (set! params (cons n params))
	   (-- spec-index)
	   (set! how (nth fspec spec-index)))
	  (#t ()))
    how))
	   

(defun format (port fspec &rest args)
  "(format how format-spec args...) print out the args according to the format-spec
   how: a port to write to,
        a string to append to
        #t to standard out
        #f return a new string
  [see the documentation for format:spec for details]"

  (let* ((string (strcpy ""))
	(format-port (cond
		      ((outputportp port) port)
		      ((stringp port)
		       (let ((p (open:string port)))
			 (seek p (length port))
			 p))
		      ((truep   port) *stdout_port*)
		      ((falsep  port) (open:string string))
		      (#t (error "format" port "port ought be a string, port, #t, or #f"))))
	(last-arg ())
	(curr-arg (car args))
	(spec-index 0)
	(spec-length (length fspec)))

    ; traipse thru' the format-spec
    (while (!= spec-index spec-length)
      (cond
       ((eq ?~ (nth fspec spec-index))
	;; do formatting
	(++ spec-index)
	(let* ((params ())
	       (got-atsign #f)
	       (got-colon  #f)
	       (how (format:parse))
	       (args-used #t))
	  (case how
		((?~)
		 (set! args-used #f)
		 (format:display-ntimes ?~ (if (nnullp params) (car params) 1)))
		((?T ?t)
		 (set! args-used #f)
		 (format:display-ntimes ?\t (if (nnullp params) (car params) 1)))
		((?|)
		 (set! args-used #f)
		 (format:display-ntimes ?\f (if (nnullp params) (car params) 1)))
		((?% ?&)
		 (set! args-used #f)
		 (format:display-ntimes ?\n (if (nnullp params) (car params) 1)))
		((?_)
		 (set! args-used #f)
		 (format:display-ntimes ?\s (if (nnullp params) (car params) 1)))
		((?\n)
		 (set! args-used #f)
		 (if got-atsign (display ?\n format-port))
		 (if (not got-colon)
		     (progn
		       ;; remove ws from fspec
		       (while (format:char-is-white (nth fspec spec-index))
			 (++ spec-index))
		       (-- spec-index))))
		((?p ?P) ; pluralize
		 (if (= 1 (if (not got-colon)
			      curr-arg
			    (set! args-used #f)
			    last-arg))
		     (if got-atsign (display "y" format-port))
		   (display (if got-atsign "ies" "s") format-port)))

		((?c ?C)
		 ((if got-atsign write display)
		      (cond
			   ((nullp params) curr-arg)
			   (#t (set! args-used #f)
			       (int->char (car params))))
			  format-port))
		
		((?<)
		 (format:format-string curr-arg display (- (car params)) ?\s ?> 'l))
		((?>)
		 (format:format-string curr-arg display (- (car params)) ?\s ?< 'r))
		((?.)
		 (format:format-string curr-arg display (- (car params)) ?\s ?> 'c))
		
		((?d ?D)
		 (format:do-number curr-arg 10 "#d"))
		((?o ?O)
		 (format:do-number curr-arg  8 "#o"))
		((?x ?X)
		 (format:do-number curr-arg 16 "#x"))
		((?b ?B)
		 (format:do-number curr-arg  2 "#b"))
		((?r ?R)
		 (if (nullp params)
		     (cond
		      ((and got-atsign got-colon)
		       ;; old style roman
		       (format:format-string-hp (roman curr-arg #t) display))
		      (got-atsign
		       ;; roman
		       (format:format-string-hp (roman curr-arg) display))
		      (got-colon
		       (format:format-string-hp (english curr-arg) display))
		      (#t
		       (format:format-string-hp (english curr-arg #t) display))))
		 (format:do-number curr-arg (let ((radx (car params)))
					      (set! params (cdr params))
					      radx) ()))
		((?a ?A)
		  (format:format-string-hp curr-arg display))
		((?s ?S)
		  (format:format-string-hp curr-arg write)))
	  (if args-used (progn
			  (set! last-arg curr-arg)
			  (set! args (cdr args))
			  (set! curr-arg (car args))))))
       (#t
	;; just copy char from fspec 
	(putc (nth fspec spec-index) format-port)))
      (++ spec-index))

    ; return desired result
    (cond
     ((outputportp port) #t)
     ((stringp port) port)
     ((truep   port) #t)
     ((falsep  port) string))))



(define format:spec 'format:spec ;; defined only for the docstring...
  "(format how fspec args...)

   how: a port to write to,
        a string to append to
        #t to standard out
        #f return a new string

  fspec: a string with embedded fomat directives of the form:
         ~[N]{,N}[v][#][@][:]X
         ie. a ~ optionally followed by a comma separated sequence of numbers
             a v will read the arg from the arglist
             a # will interpolate to the number of remaining args
             optional colon and at-sign modifiers
             and a single letter directive

         the directive can be one of:

            A  print as would display
            S  print as would write
                 @ will pad on left
                 args are: mincols (maxcols if <0), padchar, overflowchar

            ~ print a ~ [N ~s]
            T print a tab [N tabs]
            % print a newline [N newlines]
            | print a formfeed [N formfeeds]
            _ print a space [N spaces]

            P pluralize
                 @ use y/ies
                 : use previous arg

            D a number in base 10
            O a number in base 8
            X a number in base 16
            B a number in base 2
            R a number in specified radix (ie. ~7R)
                @ print leading #
                : print with commas
                args are: mincol, padchar, overflowchar, commachar, commawidth

             without a radix specifier:
                  in english \"four\"
               :  in english, ordinal \"fourth\"
               @  in roman \"IV\"
               :@ in old roman \"IIII\"


            C a character
                @ as with write
                args: numbered-character (instead of from args)


     also supported are some PERL-isms:
          ~<<<<<<<   left justified
          ~>>>>>>>   right justified
          ~.......   centered (| in perl)

            ")



