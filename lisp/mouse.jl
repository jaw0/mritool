
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"
 
;;;; $Id: mouse.jl,v 1.1 1995/11/23 04:30:35 jaw Exp jaw $

;;; format of event is:
;;; #(code flags shiftmask locx locy time action string ui)
;;; see .../xview/win_input.h


;;; some (possibly) useful defines
;;; mostly from /usr/openwin/include/xview/win_input.h

;;; event codes
(define MS_LEFT                32563 )
(define MS_MIDDLE              32564 )
(define MS_RIGHT               32565 )
(define LOC_DRAG               32515 )
			       
;;; shifmasks
(define CAPSLOCK        0      );         /* Caps Lock key                */
(define CAPSMASK        #x0001 )
(define SHIFTLOCK       1      );         /* Shift Lock key               */
(define LEFTSHIFT       2      );         /* Left-hand shift key          */
(define RIGHTSHIFT      3      );         /* Right-hand shift key         */
(define SHIFTMASK       #x000E )
(define LEFTCTRL        4      );         /* Left-hand (or only) ctrl key */
(define RIGHTCTRL       5      );         /* Right-hand control key       */
(define CTRLMASK        #x0030 )
 
(define META_SHIFT_MASK #x0040 )
(define MS_LEFT_MASK    #x0080 )
(define MS_MIDDLE_MASK  #x0100 )
(define MS_RIGHT_MASK   #x0200 )
(define MS_BUTTON_MASK  #x0380 )
(define ALTMASK         #x0400 )


(define mouse:start-x () "x coord. of the first point on the contour")
(define mouse:start-y () "y coord. of the first point on the contour")
(define mouse:last-x  () "x coord. of the most recent point on the contour")
(define mouse:last-y  () "y coord. of the most recent point on the contour")
(define mouse:mode    () "the current mouse mode")
(define mouse:selected 0 "currently selected contour")


(defun mouse:close-contour ()
  "(mouse:close-contour) close drawn contour"
  (if (and (nnullp mouse:start-x) (nnullp mouse:last-x))
      (progn
	(draw-line mouse:start-x mouse:start-y mouse:last-x mouse:last-y mouse:selected)
	(set! mouse:start-x ()))))

(define mouse:menu (mri:menu
		    (mri:button "None"       (lambda ()
					       (set! mouse:mode ())
					       (set-left-footer-text "None")))
		    (mri:button "Draw"       (lambda ()
					       (set! mouse:mode 'mouse:DRAW)
					       (set-left-footer-text "Draw")))
		    (mri:button "Break line" (lambda ()
					       (set! mouse:last-x ())
					       (set! mouse:last-y ())))
		    (mri:button "Erase"      (lambda ()
					       (set! mouse:mode 'mouse:ERASE)
					       (set-left-footer-text "Erase")))
		    (mri:button "W/L"        (lambda ()
					       (set! mouse:mode 'mouse:FIDDLE)
					       (set-left-footer-text "Adjust W/L")))
		    (mri:button "Visine"     'mri:visine)	; defined in mritool.jl
		    (mri:button "Zoom"       (lambda ()
					       (set! mouse:mode 'mouse:ZOOM)
					       (set! mouse:start-x ())
					       (set-left-footer-text "Zoom - select first corner")))
		    (mri:button "Where"      (lambda ()
					       (set! mouse:mode 'mouse:WHERE)
					       (set-left-footer-text "Loc: (x, y)")))
		    (mri:button "Snake LOI"  (lambda ()
					       (set! mouse:mode 'mouse:SELLOI)
					       (set-left-footer-text "Set LOI")))))

{defun mouse:handler (event)
  "(mouse:handler event) called from internally to handle mouse events"
  (let ((x (nth event 3))
	(y (nth event 4))
	(code (nth event 0)))
    ; (print "Mouse: " mouse:mode " " event ?\n)

    (if (or (and (= MS_LEFT  code) (=  1 (& (nth event 1) 1)))                ; left & down
	    (and (= LOC_DRAG code) (!= 0 (& (nth event 2) MS_LEFT_MASK)))     ; or left drag
	    (eq mouse:mode 'mouse:NXTSNK))
	
	(case mouse:mode
	  
	      (mouse:DRAW
	       (if (nullp mouse:last-x)
		   ;; first point
		   (progn
		     (set-left-footer-text (strcat "Draw: "
						   (nth mri:buttons mouse:selected)))
		     (set! mouse:start-x (set! mouse:last-x x))
		     (set! mouse:start-y (set! mouse:last-y y))
		     (draw-point x y mouse:selected))
		 (draw-line x y mouse:last-x mouse:last-y mouse:selected)
		 (set! mouse:last-x x)
		 (set! mouse:last-y y)))

	      (mouse:ERASE
	       (erase-area x y)
	       (set! mouse:last-x ())
	       (set! mouse:last-y ()))

	      (mouse:ZOOM
	       (cond
		((or (nullp mouse:start-x) (and (= mouse:start-x x) (= mouse:start-y y)))
		 ;; selected first corner
		 (set! mouse:start-x x)
		 (set! mouse:start-y y)
		 (set! mouse:last-x ())
		 (set-left-footer-text "Zoom - select second corner"))

		((nullp mouse:last-x)
		 ;; selected second corner
		 (set! mouse:last-x x)
		 (set! mouse:last-y y)
		 (show-box mouse:start-x mouse:start-y mouse:last-x mouse:last-y)
		 (set-left-footer-text "Zoom - click once more to confirm"))

		(#t
		 ;; confirmed
		 ;; do the zoom...
		 (set-left-footer-text "Zoom - zooming...")
		 (let ((img (img:cut (get-view) mouse:start-x mouse:start-y mouse:last-x mouse:last-y)))
		   (set-view
		    (img:zoom img (/ 512.0 (max (img:width img) (img:height img))))))
		   
		 (refresh (img:wipe (img:makeU 512 512) 0))
		 (refresh)
		 (set-left-footer-text "Zoom - done")
		 (set! mouse:mode ()))))
	      
	      (mouse:FIDDLE
	       (adjust-wl ($-$ y 256) ($-$ x 128))
	       (set-left-footer-text (strcat "W/L: "
					     (number->string ($-$ y 256))
					     "/"
					     (number->string ($-$ x 128)))))

	      (mouse:SELLOI
	       (set-left-footer-text (strcat "LOI: ("
					     (number->string x)
					     ", "
					     (number->string y)
					     ") = "
					     (number->string (mouse-set-loi x y mouse:selected)))))

	      (mouse:WHERE
	       (set-left-footer-text (strcat "Loc: ("
					     (number->string ($/$ x 1))
					     ", "
					     (number->string ($/$ y 1))
					     ")")))

	      (mouse:NXTSNK
	       ;; only reachable from clicking on measure button
	       (if (and (nnullp mouse:start-x)
			(nnullp mouse:last-x))
		   (mouse:close-contour))
	       (set! mouse:last-x ())
	       (set! mouse:last-y ())
	       (set! mouse:mode 'mouse:DRAW)
	       (set-left-footer-text (strcat "Draw: "
					     (nth mri:buttons mouse:selected))))

	      (#t
	       ))))}




  

