
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: ip.jl,v 1.1 1995/11/23 04:30:37 jaw Exp jaw $

;;; code for running in ip mode


(sets! mri:ip-mode     #t)
(sets! mri:build-pt-db #f)
(sets! mri:line-GXfnc  7)
(sets! mri:line-value  255)
(sets! mri:line-width  8)
(sets! mri:erasor      1)

(sets! prompt (lambda () (print-stderr "ip(" .lineno ") > ")))

(sets! mri:top-row
       `(
	 ("Command"  (
		      ("Function"      ( ("Show LUT"    "(show-lut)")
					 ("New Frame"   "(new-frame)")))
		      ("Help"          "(help)")
		      ("Quit"          "(quit)")))))
	 
(defvar current-image-alist ())
	 
(add-hook 'after-new-frame-hooks (lambda ()
				   ;; (print "New frame!\n")
				   (set! current-image-alist (acons
							      (car mri:frame-list)
							      ()
							      current-image-alist))))
(defun loadimage (name &optional frame)
  (if (nboundp frame)
      (set! frame (current-ui)))
  (let ((img (img:makeU 0 0))
	(sts (img:load img name))
	nc)

    (if sts (progn
	      (set-frame-size (img:width img) (img:height img))
	      (set-header-text name)
	      (set! nc (img:countcolors img))
	      (img:cannongray img)

	      ;; pop it in window
	      (refresh img)
	      ; (install-cmap img)
	      ; (if (< nc 255)
	      ;  (modify-cmap 255 #(255 0 0)))

	      ;; put it on list
	      (set-cdr! (assoc frame current-image-alist) img))
      (print "No such image\n"))
    img))

(defun displayimage (img &optional frame)
  (if (nboundp frame)
      (set! frame (current-ui)))

  (set-cdr! (assoc frame current-image-alist) img)
  (set-frame-size (img:width img) (img:height img))
  (install-cmap img)
  (refresh img))

	
(define mouse:menu
  '(("None"       "(progn (set! mouse:mode ())            (set-left-footer-text \"None\"))")
    ("Pixel"      "(progn (set! mouse:mode 'mouse:PIXEL)  (set-left-footer-text \"Pixel\"))")
    ("Fill Area"  "(progn (set! mouse:mode 'mouse:FILL)   (set-left-footer-text \"Fill\"))")
    ("Draw"       "(progn (set! mouse:mode 'mouse:DRAW)   (set-left-footer-text \"Draw\"))")
    ("Erase"      "(progn (set! mouse:mode 'mouse:ERASE)  (set-left-footer-text \"Erase\"))")))

(defun mouse:handler (event)
  "(mouse:handler event) called from internally to handle mouse events"
  (let* ((x (nth event 3))
	(y (nth event 4))
	(code (nth event 0))
	(ui (nth event 8))
	(img (cdr (assoc ui current-image-alist))))
    ; (print "Mouse: " mouse:mode " " event ?\n)

    (if (or (and (= MS_LEFT  code) (=  1 (& (nth event 1) 1)))                ; left & down
	    (and (= LOC_DRAG code) (!= 0 (& (nth event 2) MS_LEFT_MASK)))     ; or left drag
	    (eq mouse:mode 'mouse:NXTSNK))
	
	(case mouse:mode
	  
	      (mouse:DRAW
	       (if (nullp mouse:last-x)
		   ;; first point
		   (progn
		     (set-left-footer-text "Draw")
		     (set! mouse:start-x (set! mouse:last-x x))
		     (set! mouse:start-y (set! mouse:last-y y))
		     (draw-point x y mouse:selected img))
		 (draw-line x y mouse:last-x mouse:last-y mouse:selected img)
		 (set! mouse:last-x x)
		 (set! mouse:last-y y)))

	      (mouse:ERASE
	       (erase-area x y img)
	       (set! mouse:last-x ())
	       (set! mouse:last-y ()))

	      (mouse:PIXEL
	       (set-left-footer-text (strcat "Pixel: ("
					     (number->string x) ", "
					     (number->string y) ") = "
					     (number->string (img:value img x y))))
	       ; (print (current-ui) ?\n)
	       (if (and (!= mouse:last-x x) (!= mouse:last-y y))
		   (print x ?\s y ?\n))
	       
	       (set! mouse:last-x x)
	       (set! mouse:last-y y))

	      (mouse:FILL
	       (img:fill img x y)
	       (refresh img ui))


	      (#t
	   )))))



      
		

	
    
	
	