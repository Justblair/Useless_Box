totalWidth 			= 	29	;
totalHeight 		= 	20	;
materialThickness	=	2.5	;
slotThickness		=	3	;

slotNo = 4;



//dimensions();
$fn = 60;

translate ([(totalWidth - 8 * slotThickness),materialThickness,0])
	slot();

module dimensions(){
	cube ([totalWidth, totalHeight, materialThickness]);
}

module slot(){
	for (i = [0:slotNo - 1]){
		translate ([i*slotThickness*2,0,0]){
			hull(){
				cylinder (r=slotThickness/2, h = materialThickness);
				translate ([0,totalHeight - materialThickness - slotThickness/2,0])
					cylinder (r=slotThickness/2, h = materialThickness);
			}
		}
	}

}
