
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: mritool.jl,v 1.1 1995/11/23 04:30:32 jaw Exp jaw $


;;; a lot of this is cruft for backwards compatability
;;; with old version

(define patient 'patient)
(define study 'study)
(define series 'series)
(define image 'image)

(define next 'next)
(define prev 'prev)

(define mode 'mode)
(define view 'view)
(define info 'info)
(define segm 'segm)
(define contour 'contour)

(defun spt (p)   (set-patient p))
(defun sim (i)   (set-image i))
(defun shinim () (disp-l-nice (image-info)))
(defun shinpt () (disp-l-nice (patient-info)))
(defun shinse () (disp-l-nice (series-info)))
(defun shinst () (disp-l-nice (study-info)))

(define DRAWN   "=S/drawn:S/cont%I.ras")
(define CONT	"=S:S/cont.%I.ras.gz")
(define SEGM	"=G:S/segm.%I.ras.gz")


(define mri:stop-iterating #f "signal the snake to stop iterating")
(install-signal-handler SIGUSR1
			(lambda ()
			  (set! mri:stop-iterating #t)
			  (display "setting stop-iterating flag\n")))

(defmac mri:interpret-string (str)
  "(mri:interpret-string string) interprets the string
this is used extensively throught the mritool internals"
  `(catch 'eof
     (catch 'repl
       (eval (read (open:string ,str)))
       (set! .lineno (+ .lineno 1))
       (newline))))

(define mri:frame-list ()
  "a list of all of the frames we have created thus far")

(defun new-frame ()
  "(new-frame) create a new window frame, if possible"
  (if mri:windows
      (progn
	(run-hooks 'before-new-frame-hooks)
	(set! mri:frame-list (cons (new-xv) mri:frame-list))
	(run-hooks 'after-new-frame-hooks)
	(car mri:frame-list))
    ()))

(defun mri:init-frame ()
  "(mri:new-frame) called by the internals to create the initial frame"

  (set! mri:init-frame (lambda () (display "frame already initialized\n")))
  (let ((ui (if mri:windows
		 (new-frame)
	       (set! mri:frame-list (cons (new-nowin) mri:frame-list))
	       (set! mri:background #f)
	       (car  mri:frame-list))))
    (if (and mri:background (nzerop (fork)))
	(progn
	  (sleep 5)
	  (_quit)))
    (mri:init-msg)
    ui))

(defun mri:parse-X (str)
  "(mri:parse-X str) parse -XS:F -XF debugging flags from the command line"
  (let* ((ns (strsplit (substr str 2 (length str)) '( ?: )))
	(n1 (string->number (car ns))))
    (if (eq (length ns) 2)
	(set-debug-flag! n1 (string->number (cadr ns)))
      (for-each (lambda (sect)
		  (set-debug-flag! sect n1))
		(make-range 0 31)))))

;; set up default debugging flags
(mri:parse-X "-X3")
(mri:parse-X "-X4")
(mri:parse-X "-X5")
(mri:parse-X "-X6")
(mri:parse-X "-X7")

(defun disp-l-nice (l)
  "pretty print the list"
  (let ((n 0))
    (while (nnullp l)
      (if (consp (car l))
	  (print (caar l) ?\t (cdar l) ?\n)
	(print n ?\t (car l) ?\n)
	(++ n))
      (set! l (cdr l)))))

(defmac help (&optional what)
  "(help item) display the documentation for an item"
  (if (nboundp what)
      (show-help-popup)
    (if (eq what 'me)
	`(display "You are beyond help")
      `(display (docstr ,what)))))

(defun copyright ()
  "(copyright) display the copyrigt notice"
  (display "  
Copyright (c) 1993,1994,1995 Jeff Weisberg
University of Rochester
    Dept. of Electrical Engineering
    Dept. of Neurology, C.E.P.
see the file \"License\" or type \"(license)\" at the prompt
"))

(defun license ()
  "(license) display the software license"
  (cat-file mri:license))

(defun mri:visine ()
  "(mri:visine) get the red out"
  ;; called from mouse menu
  (visine)
  (set! mouse:last-x ())
  (set! mouse:last-y ())
  (refresh))

(defun show (what &rest argl)
  "(show ...) show various parameters,
such as: patients, studies, series, images, info, stats"
  (case what
	((patient patients pat pats pt pts)
	 (disp-l-nice (patient-list)))
	((studies study stdy std)
	 (disp-l-nice (study-list)))
	((series ser sers srs)
	 (disp-l-nice (series-list)))
	((images image img imgs)
	 (disp-l-nice (image-list)))
	((stats stat statistics)
	 (show-stats))
	((loi lois)
	 (disp-l-nice (loi-list)))
	(info
	 (case (car argl)
	       ((patient patients pat pats pt pts)
		(disp-l-nice (patient-info)))
	       ((studies study stdy std)
		(disp-l-nice (study-info)))
	       ((series ser sers srs)
		(disp-l-nice (series-info)))
	       ((image images img imgs)
		(disp-l-nice (image-info)))
	       (#t
		(display "show info: must specify one of: patient, study, series, image\n"))))
	(#t
	 (display "show: must specify one of: patients, studies, series, images, info, stats, loi\n"))))


(defun set (what &rest argl)
  "(set ...) set various parameters,
such as patient, study, series, image"
  (case what
	((patient patients pat pats pt pts)
	 (set-patient (car argl))
	 (run-hooks 'set-patient-hooks)
	 (if (numberp (and (consp (cdr argl)) (cadr argl)))
	     (apply 'set 'study (cdr argl))))

	((studies study stdy std)
	 (set-study (car argl))
	 (run-hooks 'set-study-hooks)
	 (if (numberp (and (consp (cdr argl)) (cadr argl)))
	     (apply 'set 'series (cdr argl))))

	((series ser sers srs)
	 (set-series (car argl))
	 (run-hooks 'set-series-hooks)
	 (if (numberp (and (consp (cdr argl)) (cadr argl)))
	     (apply 'set 'image (cdr argl))))

	((images image img imgs)
	 (set-image (car argl))
	 (run-hooks 'set-image-hooks))
	
	((loi lois)
	 (set-loi argl))
	(mode
	 (case (car argl)
	       (segm
		(set-mode-segm))
	       (view
		(set-mode-view))
	       (#t
		(display "set mode: must specify one of: segm, view\n"))))
	(#t
	 (display "set: must specify one of: patients, studies, series, images, loi, mode\n"))))


(defun save (what &optional f t n)
  "(save what [from to] [filename]) save things, segm, contour, view"
  (let
      ((from (if (intp f) f (current-image)))
       (to   (if (intp t) t (current-image)))
       (file (if (stringp n) n
	       (if (stringp f) f
		 ()))))
       (case what
	     ((segment segm)
	      (save-segment from to 
			    (if (nullp file) SEGM file)))
	     ((contour cont)
	      (save-contour from to
			    (if (nullp file) CONT file))))))

(defun mri:histogram ()
  "(mri:histogram) plot histogram"
  (let* ((h (img:histogram (get-view)))
	 (max (let ((x 0))
		(dolist (v h)
		  (if (> v x)
		      (set! x v)))
		x)))
    (dotimes (i 256)
      (set-nth! h i (/ (* 256.0 (nth h i)) max)))
    (graph h))
  (show-graph-popup))

(defun mri:set-header ()
  "(mri:set-header) sets the frame header"
  ;; called by XV::done_thinking()
  (set-header-text (strcat "(" mri:versn ")   "
			   (mriname "P%: %P   T%: %T   S%: %S   I%: %I"))))


;; functions for displaying multiple images

(defun mri:makblock (w h &rest limg)
  "(mri:makblock width height img1 img2 ...) make images into a block"
  (let ((nimg (img:makeU (* 256 w) (* 256 h)))
	(x 0)
	(y 0))
    (dolist (l limg)
      (img:overlay nimg l (* x 256) (* y 256))
      (++ x)
      (if (= x w)
	  (progn
	    (set! x 0)
	    (++ y))))
    nimg))

(defun mri:displayimage (img (ui (current-ui)))
  "(mri:displayimage img [ui]) display an image"
  (set-view img ui)
  (set-frame-size (+ 2 (img:width img)) (+ 47 (img:height img)) ui)
  (set-current-ui! ui)
  (refresh img ui))

(defun mri:display-range (l h (u (current-ui)))
  "(mri:display-range lo high [ui]) display a block of images"
  (let ((li ())
	(n  l)
	(t (+ 1 (- h l)))
	x y)
    (while (<= n h)
      (set image n)
      (set! li (append li (list (img:zoom (get-view) .5))))
      (++ n))
    (cond
     ;; choose best size for frame
     ((<= t 1) (set! x 1)(set! y 1))
     ((<= t 2) (set! x 1)(set! y 2))
     ((<= t 4) (set! x 2)(set! y 2))
     ((<= t 6) (set! x 3)(set! y 2))
     ((<= t 8) (set! x 4)(set! y 2))
     ((<= t 9) (set! x 3)(set! y 3))
     (#t (set! x 4)(set! y 3)))
    (mri:displayimage (apply mri:makblock x y li) u)
    (set-right-footer-text (strcat "Displaying slices "
				   (number->string l)
				   " through "
				   (number->string h)) u) ))

(defun mri:display-range-nf (l h)
  "(mri:display-range-nf low high) display a block of images in a new frame)"
  (let ((u (new-frame)))
    (set-current-ui! u)
    (mri:display-range l h u)))

(defun mri:display-series ()
  "(mri:display-series) display the entire series in blocks of 12"
  (let* ((ni (cdr (assv 'images (series-info))))
	 (nf (/ (+ ni 11) 12))
	 (ui (current-ui))
	 (uv (makevector nf)))
    (dotimes (i nf)
      (set-nth! uv i (new-frame)))
    (dotimes (i nf)
      (set-current-ui! (nth uv i))
      (mri:display-range (+ (* i 12) 1) (min ni (* (+ i 1) 12)) (nth uv i)))))


;; colors...
(defun mri:lsd ((k0 5) (k1 4) (k2 9) (k3 0) (k4 0) (k5 0))
  "(mri:lsd [upto 5 numbers]) install psychadelic color map"
  (let (r g b)
    (dotimes (n 64)
      (set! r (->int (% (* (+ k3 n) 4 k0) 256)))
      (set! g (->int (+ (* (sin (/ (* k1 (+ k4 n) 3.14) 64)) 128) 128)))
      (set! b (->int (+ (* (cos (/ (* k2 (+ k5 n) 3.14) 64)) 128) 128)))
      (modify-cmap (+ n 64) (vector r g b)))
    (modify-cmap #t)))

;; analysis
(defun mri:analyze-all ()
  "(mri:analyze-all) analyze all objects"
  (let ((n 0)
	(dolist (i mri:buttons)
	  (print "Analyzing " i ?\n)
	  (analyze 0 100 n)
	  (++ n))))
  (show-stats))

;; image get method
;; we can now use foo,width to get the width,...
(defun img:get (this huh)
  "(img:get img param) get data about image"
  (case huh
    ((width)     this,(width))
    ((height)    this,(height))
    ((min)       this,(minval))
    ((max)       this,(maxval))
    ((mean)      this,(mean))
    ((variance)  this,(variance))
    ((histogram) this,(histogram))
    ((clone)     this,(clone))
    ((copy)      this,(cpy))))

(defun img:print (this &optional port)
  "(img:print img) image print method"
  (print-with-port port "#<" this,width " by " this,height " image>"))

;;; simple functions to assist in making our buttons and menus, ...

(defun mri:button (label action)
  "(mri:button label action) create a button"
  (list label action))

(defun mri:menu items
  "(mri:menu items...) create a menu"
  items)

(defun mri:add-pulldown (menu)
  "(mri:add-pulldown menu) add a menu to the top row"
  (set! mri:top-row (cons menu mri:top-row)))

(define mri:add-pulldown-left
  "(mri:add-pulldown-left menu) add a menu to the left on the top row"
  mri:add-pulldown)

(defun mri:add-pulldown-right (menu)
  "(mri:add-pulldown-right menu) add a menu to the right on the top row"
  (set! mri:top-row (append mri:top-row (list menu))))


(load "mouse.jl")
(load "postscript.jl")

  



