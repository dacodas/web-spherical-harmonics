// (let ((coordinates (make-array (list (1+ (* steps (1+ steps))) 2) :element-type 'float)))
//             (loop
//                for theta-step upto steps
//                for theta = 0.0 then (incf theta (/ 1.0 steps))
//                do (loop
//                      for phi-step upto steps
//                      for phi = 0.0 then (incf phi (/ 1.0 steps))
//                      do (setf (aref coordinates (+ (* steps theta-step) phi-step) 0) theta
//                               (aref coordinates (+ (* steps theta-step) phi-step) 1) phi)))
//             (let ((elements (make-array (* steps steps 2) :element-type 'float))
//                   (counter -1))
//               (loop for step below steps
//                  do (loop for stepito below steps
//                        do (setf (aref elements (incf counter)) (+ (* steps step) stepito)
//                                 (aref elements (incf counter)) (+ (* steps (+ step 1)) stepito))))
//               (list coordinates elements)))

1 + ( steps * (steps + 1) )
