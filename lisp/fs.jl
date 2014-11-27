

(defun ll ()
  (let ((dirlst (getdirents ".")))
    (for-each (lambda (dirent)
		(let ((in (stat (car dirent))))
		  (format #t
"~-4o ~3d ~>>>>>>> ~<<<<<<< ~-7d ~A\n"
			  (% (nth in 1) #o777)
			  (nth in 2)
			  (nth (getpwuid (nth in 3)) 0)
			  (nth (getgrgid (nth in 4)) 0)
			  (nth in 5)
			  (car dirent))))
	      dirlst)))

