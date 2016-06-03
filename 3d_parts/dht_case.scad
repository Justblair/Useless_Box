dht_height  =   25.1    ;
dht_body_h  =   20      ;
dht_width   =   15.1    ;
dht_length  =   7.7     ;
dht_hole_d  =   3.4     ;

wall_thickness = 3         ;
wall_width = 43			;
wall_height = 33		;
wall_depth = 23			;

difference(){
    walls();
    translate([dht_height+2,dht_width + wall_thickness + 2.5 ,wall_thickness])
        rotate([90,0,270])
            #fake_dht();
}

module walls(){
    difference(){
        cube([wall_width,wall_height, wall_depth]);
        translate([-wall_thickness,wall_thickness,wall_thickness])
            cube([wall_width,wall_height, wall_depth]);
		translate([0,wall_depth - 5,wall_depth - 1.6])
			cube ([50,6.5,10]);
    }
    
}

module fake_dht(){
    cube ([dht_width, dht_length, dht_body_h]);
    //difference(){
        cube ([dht_width, 1, dht_height]);
    //    translate([dht_width/2,1.5,dht_height - (dht_height - dht_body_h)/2])
    //        rotate([90,0,0])
    //            cylinder(r=dht_hole_d/2, h = 10, $fn=20);
    //}
}
