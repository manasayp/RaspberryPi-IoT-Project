rm /var/www/html/temperatures.csv
 while true; do ./motionSensor >> /var/www/html/motion.csv; sleep 1; done


