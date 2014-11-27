#! jl -f

(let* ((in (open:read "| mail -H"))
      (line (getline in))
      (ln 1))

  (while (nnullp line)
    (let* ((f (strsplit line '(?\s ?\t ?\n)))
	   (nf (length f))
	   (i 0))
      (while (and (< i 3) (not (match "@" (nth f i))))
	(++ i))
      (format #t "~>> ~<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< -- " ln (nth f i))
      (set! i (+ i 6))
      (while (!= i nf)
	(format #t "~A " (nth f i))
	(++ i)))
    (newline)
    (set! line (getline in))
    (++ ln)))





