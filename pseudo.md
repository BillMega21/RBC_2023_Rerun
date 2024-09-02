# 1. Traverse all, save shortcut along the way, stop at DESTINATION
while (currentColour == red and currentCoordinate != startCoordinate){
	
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    1. Identify current state()
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	- update current coordinate
		- 
	- update current distance
	- update current color
	- update explored cells
	- update special cells
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    2. 	Plan_Next_step & orientate()
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	- Use ultrasonic to find free path
	- Direct robot to free path
  

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    3. Move()
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	- Move FORWARD 1 step
		- Feedback loop to adjust two wheels' speed (HARD TO CALIBRATE - LAST STUFF)
		* Check distance from 2 sides
		* Go to the one further from the wall
	    
}

    2. Flash the LED 3 time, 1-1s

	3. Compute the best shortcut, Backtrack
    
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Find optimal back track
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            	- Iterate through all pairs of shortcuts:
            		- Optimal_array = (Distance to that shortcut) - (Shortcut_gap))
                    - NOTE: length array = num of pairs
                - Get the minimum value from optimal_array
                - Get the index
                - Store the "distance" of that shortcut point
                
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Move 
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            	- 180 degree turn
                - NEW FUNCTION to get which array index, base on
                	- cell's "distance" value
                    - current location
                - *kind of* repeat the STEP 1 - identify current state > plan & turn > move
                	- Move toward the OPTIMAL SHORTCUT CELL
                    - Identify color & send BLUETOOTH signal (please, EDEN, get this done correctly)
                    - LED and wait for gate?
                    - Traverse thru - is it the same thing again?
                    	- Go thru a GATE, not go through a FREE PATH
                - Repeat till reach the START
                - Get the money $$$$$$
