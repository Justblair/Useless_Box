totalWidth 			= 	40	;
totalHeight 		= 	30	;
materialThickness	=	2.5	;
slotThickness		=	3	;

slotNo = 6;


%cube ([totalWidth,totalHeight, slotThickness]);
//dimensions();
$fn = 60;

translate ([(slotThickness*slotNo*2)/slotNo - slotThickness/2, materialThickness + .5,0])
	slot();

module dimensions(){
	cube ([totalWidth, totalHeight, materialThickness]);
}

module slot(){
	for (i = [0:slotNo - 1]){
		translate ([i*slotThickness*2,0,0]){
			hull(){
				cylinder (r=slotThickness/2, h = materialThickness);
				translate ([0,totalHeight - materialThickness - slotThickness/2 - 1, 0])
					cylinder (r=slotThickness/2, h = materialThickness);
			}
		}
	}

}
