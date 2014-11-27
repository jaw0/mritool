
;; $Id: slib.jl,v 1.1 1995/11/23 04:30:33 jaw Exp jaw $
;; some code borrowed from slib / jaffer



;; from slib/jaffer
(defun remove (i l)
  "(remove item list) remove item (equal) from list"
  (cond
   ((nullp l) l)
   ((equal i (car l))
    (remove i (cdr l)))
   (#t (cons (car l) (remove i (cdr l))))))

(defun remq (i l)
  "(remq item list) remove item (eq) from list"
  (cond
   ((nullp l) l)
   ((eq i (car l))
    (remq i (cdr l)))
   (#t (cons (car l) (remq i (cdr l))))))

(defun remv (i l)
  "(remv item list) remove item (eqv) from list"
  (cond
   ((nullp l) l)
   ((eqv i (car l))
    (remv i (cdr l)))
   (#t (cons (car l) (remv i (cdr l))))))


(defun delete! (i l)
  "(delete! item list) destructively delete item (equal) from list"
  (cond
   ((nullp l) ())
   ((equal i (car l)) (delete! i (cdr l)))
   (#t (set-cdr!  l (delete! i (cdr l)))
       l)))

(defun delq! (i l)
  "(delq! item list) destructively delete item (eq) from list"
  (cond
   ((nullp l) ())
   ((eq i (car l)) (delq! i (cdr l)))
   (#t (set-cdr!  l (delq! i (cdr l)))
       l)))

(defun delv! (i l)
  "(delv! item list) destructively delete item (eqv) from list"
  (cond
   ((nullp l) ())
   ((eqv i (car l)) (delv! i (cdr l)))
   (#t (set-cdr!  l (delv! i (cdr l)))
       l)))

