
;;;; Copyright (c) 1994 Jeff Weisberg
;;;; see the file "License"

;;;; $Id: mrirc.cf.jl,v 1.1 1995/11/23 04:30:31 jaw Exp jaw $
;;;; system wide config file for mritool program

; xdef name default-value type-predicate preprocess-command docstrings
(define xdef xdefine)

(xdef mri:across 2 intp () "number of buttons across")
(xdef mri:buttons                                        ; the button labels
  '("R Temporal"        "L Temporal"
    "R White"           "L White"
    "R Grey"            "L Grey"
    "R Amygdala"        "L Amygdala"
    "R Hippocampus"     "L Hippocampus"
    "R Parahippocampus" "L Parahippocampus"
    "R Horn"            "L Horn"
    "R Brain"           "L Brain")
  listp () "a list of labels for the buttons")


(xdef mri:top-row () listp () "a list describing the buttons and menus along the top of the tool")

(mri:add-pulldown (mri:button "Functions"
			      (mri:menu
			       (mri:button "Analyze"    'mri:analyze-all)
			       (mri:button "Archiving"
					   (mri:menu
					    (mri:button "Mark Patient" 'rdb:mark-patient)
					    (mri:button "Mark Study"   'rdb:mark-study)
					    (mri:button "Mark Series"  'rdb:mark-series)
					    (mri:button "Mark Image"   'rdb:mark-image)
					    (mri:button "Archive marked items" 'rdb:arcv)
					    (mri:button "Delete marked items"
							(mri:menu
							 (mri:button "No, don't delete" ())
							 (mri:button "Yes, really delete" 'rdb:dele)))))
			       (mri:button "Help"          'help)
			       (mri:button "License"       'show-license-popup)
			       (mri:button "Save Contour"  "(save contour)")    ; defaults to CONT
			       (mri:button "Load Contour"  (lambda ()
							     (load-cont CONT)
							     (refresh)))
			       (mri:button "Snake"         (lambda ()
							     (show-snake-popup)
							     (show-measure-popup)
							     (show-bounds-popup)))
			       (mri:button "Segment"       (lambda ()
							     (show-segm-popup)
							     (show-bounds-popup)))
			       (mri:button "Quit"          quit))))

(mri:add-pulldown (mri:button "Display"
			      (mri:menu
			       (mri:button "View Modes"
					   (mri:menu
					    (mri:button "Image"       "(set mode view)")
					    (mri:button "Segm"        "(set mode segm)")))
			       (mri:button "Show LUT"             'show-lut)
			       (mri:button "Pseudo Color"         'mri:lsd)
			       (mri:button "Display Whole Series" 'mri:display-series)
			       (mri:button "Histogram"            'mri:histogram)
			       (mri:button "New Frame"            'new-frame)
			       (mri:button "Refresh"       'refresh)
			       (mri:button "Visine"        (lambda ()
							     (visine)
							     (refresh))))))

(mri:add-pulldown (mri:button  "Select"
			       (mri:menu
				(mri:button "Patients"      'show-patient-popup)
				(mri:button "Next Image"    "(set image next)")
				(mri:button "Prev Image"    "(set image prev)")
				(mri:button "Select Image"  'show-image-popup))))


;;;    locate some important files and such
(xdef mri:rdb:host  "gyrus.neurology.rochester.edu"  stringp ()      "the remote database server host")
(xdef mri:rdb:port  1928                             ()      ()      "the remote database server port")
(xdef mri:patdir    (mriname "%PATDIR%")             stringp mriname "where the mri-scans are")
(xdef mri:segmdir   (mriname "%SEGMDIR%")            stringp mriname "where the segm data is kept")
(xdef mri:etcdir    (mriname "%ETCDIR%")             stringp mriname "where some goodies are")
(xdef mri:savedir   (mriname "~/data/save")          stringp mriname "where to save stuff")
(xdef mri:debugdir  (mriname "/data/mri/debug")      stringp mriname "where to save debugging stuff")
(xdef mri:savename  "Datafile"                       stringp ()      "the name of the data file")  ; not used -- anachr.
(xdef mri:statefile ".mri-state"                     stringp ()      "the name of the state file") ; not used -- anachr.
(xdef mri:helpfile  (strcat mri:etcdir "/help.txt")  stringp mriname "the help file")
(xdef mri:license   (strcat mri:etcdir "/License")   stringp mriname "the license file")
(xdef mri:patients  ""                               stringp mriname "list of patients for this project")

;;; various parameters
;; aesthetics
(xdef mri:line-width   2    intp () "the width of lines you draw")
(xdef mri:line-style   1    intp () "line style: 1=solid, 2=dashed, 3=double-dashed")
(xdef mri:line-value   128  intp () "value of drawn lines")
(xdef mri:line-GXfnc   7    intp () "GX function: GXxor=6, GXset=15, GXor=7,... (see X.h)")
(xdef mri:erasor       16   intp () "size of erasor")
(xdef mri:cursor-size  1    intp () "size of the cursor")
;; window positions
(xdef mri:base_x       600  intp () "the x position of the base frame")
(xdef mri:base_y       7    intp () "the y position of the base frame")
(xdef mri:child_x      0    intp () "the x position of the child frame")
(xdef mri:child_y      0    intp () "the y position of the child frame")
(xdef mri:child_dx     20   intp () "the x delta of the child frame")
(xdef mri:child_dy     20   intp () "the y delta of the child frame")
(xdef mri:ptl_w        275  intp () "the width of the patient list")
(xdef mri:ptl_h        425  intp () "the height of the patient list")
(xdef mri:ptl_x        7    intp () "the x position of the patient list")
(xdef mri:ptl_y        28   intp () "the y position of the patient list")
(xdef mri:ptl_r        20   intp () "the number of rows on the patient list")
(xdef mri:stl_w        225  intp () "the width of the study list")
(xdef mri:stl_h        425  intp () "the height of the study list")
(xdef mri:stl_x        140  intp () "the x position of the study list")
(xdef mri:stl_y        86   intp () "the y position of the study list")
(xdef mri:stl_r        20   intp () "the number of rows on the study list")
(xdef mri:sel_w        383  intp () "the width of the series list")
(xdef mri:sel_h        425  intp () "the height of the series list")
(xdef mri:sel_x        3    intp () "the x position of the series list")
(xdef mri:sel_y        180  intp () "the y position of the series list")
(xdef mri:sel_r        20   intp () "the number of rows on the series list")
(xdef mri:iml_w        145  intp () "the width of the image list")
(xdef mri:iml_h        425  intp () "the height of the image list")
(xdef mri:iml_x        125  intp () "the x position of the image list")
(xdef mri:iml_y        425  intp () "the y position of the image list")
(xdef mri:iml_r        20   intp () "the number of rows on the image list")
(xdef mri:snk_x        335  intp () "the x position of the snake popup")
(xdef mri:snk_y        623  intp () "the y position of the snake popup")
(xdef mri:rng_x        335  intp () "the x position of the range popup")
(xdef mri:rng_y        112  intp () "the y position of the range popup")
(xdef mri:sgm_x        335  intp () "the x position of the segm popup")
(xdef mri:sgm_y        440  intp () "the y position of the segm popup")
(xdef mri:slt_x        335  intp () "the x position of the selection popup")
(xdef mri:slt_y        195  intp () "the y position of the selection popup")
(xdef mri:hst_x        330  intp () "the x position of the histogram popup")
(xdef mri:hst_y        5    intp () "the y position of the histogram popup")

;; default function parameters
(xdef mri:elastic      20   intp () "default snake elasticity")
(xdef mri:attractive   15   intp () "default snake atractiveness")
(xdef mri:stiffness    100  intp () "default snake stiffness")
(xdef mri:3dconstraint 0    intp () "default snake 3D term")
(xdef mri:similarity   5    intp () "default snake similararity")
(xdef mri:maxiter      100  intp () "max # of iters when minimizing snake")
(xdef mri:percent      90   intp () "max # of iters when minimizing snake")

(xdef mri:nlev           6    intp () "default number of segm levels")
(xdef mri:alpha          1024 intp () "default segm alpha value")
(xdef mri:segm:maxiter-k 50   intp () "max number of kmean iterations")
(xdef mri:segm:maxiter-g 500  intp () "max number of gibbs iterations")

(xdef mri:window       230  intp () "default window")
(xdef mri:level        -15  intp () "default level")

(xdef mri:build-pt-db  #t   booleanp () "build the patient database")
(xdef mri:ip-mode      #f   booleanp () "run in IP mode")
(xdef mri:rdb:doit-p   #f   booleanp () "read files over remote connection?")

(sets! prompt (lambda () (print-stderr "mri(" .lineno ") > ")))

(xdef mri:init-msg
  (lambda ()
    (display mri:version) (newline)
    (display .version)    (newline)
    (copyright)           (newline) (newline))
  procedurep () "thunk to display initialization message")


