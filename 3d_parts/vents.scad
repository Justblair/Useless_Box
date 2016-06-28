totalWidth 			= 	40	;
totalHeight 		= 	30	;
materialThickness	=	2.5	;
slotThickness		=	3.4	;

backWidth			=	104.9;
backheight			=	55.8;

	$fn = 60;
hull(){
translate ([backWidth/4,materialThickness + 3.3/2 + 1 ,0])
	cylinder (r = 3.3/2, h = materialThickness);
translate ([backWidth/4,0,0])
	cylinder (r = 3.3/2, h = materialThickness);
}

// slotNo = 6;
// difference(){
// cube ([backWidth, backheight, materialThickness]);



// translate([backWidth - materialThickness - totalWidth, materialThickness, 0]){
	// %cube ([totalWidth,totalHeight, slotThickness]);

	dimensions();

	// translate ([(slotThickness*slotNo)/slotNo - slotThickness/2 + 1, slotThickness/2,0]){
		// slot();}
	// }
// }

	// translate([backWidth - 10, 22 ,0])
	// %cube ([10,10, slotThickness]);

	
	
// module dimensions(){
	// cube ([totalWidth, totalHeight, materialThickness]);
// }

// module slot(){
	// for (i = [0:slotNo - 1]){
		// translate ([i*slotThickness*2,0,0]){
			// hull(){
				// cylinder (r=slotThickness/2, h = materialThickness);
				// if (i > 3){
				// translate ([0,totalHeight - slotThickness - 15, 0])
					// cylinder (r=slotThickness/2, h = materialThickness);
					// } else {
				// translate ([0,totalHeight - slotThickness, 0])
					// cylinder (r=slotThickness/2, h = materialThickness);					
					// }
			// }
		// }
	// }

// }
