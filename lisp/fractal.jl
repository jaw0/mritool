
(gc)

(define H 3.3 "fractal dimension")
(define SZ 256)
(define WSZ 4)

(defun spit (img)
  (if (imagep img)
      (let ((w (img:width img))
	    (h (img:height img)))
	(dotimes (y h)
	  (dotimes (x w)
	    (print (img:value img x y) ?\s))))

    (let ((vi (solid->vector img))
	  (l  (length vi)))
      (dotimes (i l)
	(spit (nth vi i))
	(newline)))))

(defun plothist (hist)
 (let ((min ())
       (max ())
       (l (length hist))
       v
       k)

   (dotimes (i l)
     (set! v (nth hist i))
     (if ((or nullp min) (< v min))
	(set! min v))
     (if (or (nullp max) (> v max))
	 (set! max v)))
   (set! k (/ 128.0 (- max min)))
   (dotimes (i l)
     (set-nth! hist i
	       (* (- (nth hist i) min) k)))
   (graph hist)))

(defun stats (img)

  #|
  (let ((min ())
	(max ())
	(w (- (img:width img)  WSZ))
	(h (- (img:height img) WSZ))
	val
	(n 0)
	(m 0.0)
	(m2 0.0))

    (dotimes (y h)
      (dotimes (x w)
	(if (and (> x WSZ) (> y WSZ))
	    (progn
	      (set! val (img:value img x y))
	      (++ n)
	      (if (or (nullp min) (< val min))
		  (set! min val))
	      (if (or (nullp max) (> val max))
		  (set! max val))
	      (set! m (+ m val))
	      (set! m2 (+ m2 (* val val)))))))
    
    (print "Min:  " min ?\n)
    (print "Max:  " max ?\n)
    (print "Mean: " (/ m n) ?\n)
    (print "...:  " m ?\s m2 ?\s n ?\n)
    (print "Var:  " (/ (- m2 (* m m)) (* n n)) ?\n))
  |#

  (print
   "Min  : " (img:minval img) ?\n
   "Max  : " (img:maxval img) ?\n
   "Mean : " (img:mean   img) ?\n
   "Var  : " (img:variance img) ?\n))

(defun fract-test (sz h wz)
  (let* ((fi (img:makeF sz sz))
	 (adj (img:clone fi))
	 fd fs prj rs ri rd st)

    (img:fractal fi h)
    (set! fd (img:cpy fi))
    (img:quantize fd 255)
;(displayimage fd)
    (gc)
    (img:wipe adj wz)
    (img:+= fd adj)
    (set! fs (2D->3D fd (+ (* wz 2) 256)))
    (set! rs (fractal-analyze fs wz))
    (gc)
    (set! prj (xyz-projections rs))
    (set! ri (nth prj 4))
    (set! rd (img:cpy ri))
    (img:punch rd 0 4)
;(displayimage rd)
    (gc)

    (set! st (list
	      (img:minval   ri)
	      (img:maxval   ri)
	      (img:mean     ri)
	      (img:variance ri)))
    st))

#|


;; create fractal
(define fi (img:makeF SZ SZ))
(img:fractal fi H)

(define fd (img:cpy fi))
(img:quantize fd)
(displayimage fd)
(img:save fd (strcat "=w:d/fracto:" (number->string H) ".ras"))

(gc)

;; convert to surface in 3 space
(define adj (img:clone fd))
(img:wipe adj WSZ)
(img:+= fd adj)
(define fs (2D->3D fd (+ (* WSZ 2) 256)))

;; find dimension
;; (set-debug-flag! SECTION_LISP DBG_VERBOSE)
(define rs (fractal-analyze fs WSZ))

(gc)

;;(spit rs) (newline)

;; flatten back to 2 space
(define prj (xyz-projections rs))
(define ri (nth prj 4))
(img:save ri (strcat "=w:d/fractr:" (number->string H) ".ras"))

(gc)

(define rd (img:cpy ri))
(img:punch rd 0 4)
(displayimage rd)

(define rq (img:cpy ri))
(img:quantize rq 255)
(displayimage rq)

(gc)

;;(spit ri) (newline)

(define rh (img:histogram rd))
(print "0 was: " (nth rh 0) ?\n)
(set-nth! rh 0 0)
(show-graph-popup)
(plothist rh)
(print "Histo: " rh ?\n)

(stats ri)

(gc)
|#


