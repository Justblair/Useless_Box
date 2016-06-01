dht_height  =   25.1    ;
dht_body_h  =   20      ;
dht_width   =   15.1    ;
dht_length  =   7.7     ;
dht_hole_d  =   3.4     ;

wall_width = 2          ;

difference(){
    walls();
    translate([dht_height+2,dht_width + wall_width + 2.5 ,wall_width])
        rotate([90,0,270])
            #fake_dht();
}

module walls(){
    difference(){
        cube([dht_height + 4+wall_width,dht_width + wall_width + 5, dht_length + 5]);
        translate([-wall_width,wall_width,wall_width])
            cube([dht_height + 4+wall_width,dht_width + wall_width + 5, dht_length + 5]);
    }
    
}

module fake_dht(){
    cube ([dht_width, dht_length, dht_body_h]);
    //difference(){
        cube ([dht_width, 1, dht_height]);
        translate([dht_width/2,1.5,dht_height - (dht_height - dht_body_h)/2])
            rotate([90,0,0])
                cylinder(r=dht_hole_d/2, h = 10, $fn=20);
    //}
}
