
(set! .gc_thresh (* .box_size 500))
(defun rand ()
  (/ (random) 268435455.0))		; this number is magic...

(defun fractal-1 (l h)
  (let* ((img (img:makeC l 1))
	 (v (makevector l))
	 (hp (- (+ h .5)))
	 (k  (* 2 pi))
	 t a p)

      (dotimes (i l)
	(set! t 0)
	(dotimes (j 12)			; 12 terms for our Normal pdf
	  (set! t (+ t -.5 (rand))))
	(set! a (if (zerop i)
		    0			; DC = 0
		  (* t (exp (* hp (log i))))))
	(set! p (* k (rand)))
	(print a "/_" p ?\n)
	(img:cput img i 0 (* a (cos p)) (* a (sin p))))

      (img:ifft img)
      
      (dotimes (i l)
	(print (img:value img i 0) ?\n)
	(set-nth! v i (img:value img i 0)))
      v))

(defun fractal-2 (w h f)
    (let* ((img (img:makeC w h))
	 (hp (- (+ h .5)))
	 ;; (c  (* hp hp))
	 (k  (* 2 pi))
	 (w2 (/ w 2))
	 (h2 (/ h 2))
	 t re im a p xx yy)

      (dotimes (y h)
	(print ?\n)
	(dotimes (x w)
	  (set! t 0)
	  (dotimes (j 12)
	    (set! t (+ t -.5 (rand))))
	  (set! xx (if (> x w2) (- w x) x))
	  (set! yy (if (> y h2) (- h y) y))
	  (set! a (* t (exp (* hp (log (+ 2 (* 2 (+ xx yy))))))))
	  (set! p (* k (rand)))
	    
	  ;; (set! t 0)
	  ;; (dotimes (j 12)
	  ;;   (set! t (+ t -.5 (rand))))
	  ;; (set! im (* t (exp (* hp (log (* (+ 1 x) (+ 1 y)))))))
	  ;; (print re " + j " im ?\n)

	  (print a " /_ " p ?\n)
	  (img:cput img x y (* a (cos p)) (* a (sin p)))))
      (img:ifft img)

      (img:quantize img 255)
      (img:save img "~/tmp/foo.ras")
      (displayimage img)
      img))


	  