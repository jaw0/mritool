

(defmac with-output (out &rest body)
  (let ((*stdout_port* (eval out)))
    (display body *stderr_port*) (newline)
    (eval (cons progn body))))
  
(defun julia (cx cy n)
  "(julia cx cy n) draw julia set"
  (let ((zx 0)
	(zy 0)
	x
	y
	s
	(i 0)
	(out (open:write "| graph -g0 -m0 -s | plot")))
    (while (!= i n)
      (set! zx (- zx cx))
      (set! zy (- zy cy))
      (set! s (sqrt (+ (* zx zx) (* zy zy))))
      (set! x (/ (+ s zx) 2.0))
      (set! y (/ (- s zx) 2.0))
      (if (< zy 0) (set! y (- y)))
      (set! zx x)
      (set! zy y)
      (if (> (random) 500000)
	  (progn
	    (set! zx (- zx))
	    (set! zy (- zy))))
      (format out "~A~T~A~%" zx zy)
      (set! i (+ i 1)))
    (close out)))





