
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: postscript.jl,v 1.1 1995/11/23 04:30:38 jaw Exp jaw $


(define ps:decorations ())

;; note: slides are 11 x 7.33; @ 128.151.24.47
(defun mri:postscript (file xdim ydim (border .5) (extras ps:decorations))
  "(mri:postscript file width height [border [extras]]) create postscript"
  (let* ((img (get-view))
	 (cnt (get-contour-v))
	 (w   (current-window))
	 (l   (current-level))
	 (t   (localtime))
	 (a   (max (min (- l (/ w 2)) 255) 0))
	 (b   (max (min (+ l (/ w 2)) 255) 0)))

#|    (if (nullp extras)
	(set! extras (list (format #f
			     "xdim 2 div xpix -15 ypix moveto 10 tr (mritool ~D/~D/~D  ~D) show"
			     (+ (nth t 5) 1900) (nth t 4) (nth t 3)
			     (mriname "%P.%T.%S.%I")))))
|#
    (display extras)
    (img:punch img a b 0 128)
    (img:|= img cnt)
    (img:ps img file xdim ydim border extras)))


(defun mri:ps:defaults ()
  "(mri:ps:defaults) reset decorations"
  (set! ps:decorations ()))

(defun mri:ps:title (txt)
  "(mri:ps:title title) place a title on the postscript output"

  (set! ps:decorations (cons
			(strcat ?( txt ?)
				" 20 tr dup stringwidth pop width exch sub 2 div height 30 sub moveto show")
			ps:decorations)))

(defun mri:ps:label (txt x y)
  "(mri:ps:label txt x y) place a label on the postscript output"

  (set! ps:decorations (cons
			(strcat ?( txt ?) ?\s
				       (number->string x) " xpix "
				       (number->string y) " ypix  moveto 10 tr show")
			ps:decorations)))

(defun mri:ps:arrow (xo yo xi yi)
  "(mri:ps:arrow xo yo xi yi) place an arrow on the postscript output"

  (set! ps:decorations (cons
			(strcat (number->string xo) " xpix "
				(number->string yo) " ypix "
				(number->string xi) " xpix "
				(number->string yi) " ypix ptr")
			ps:decorations)))


