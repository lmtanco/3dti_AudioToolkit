void setup()
{
  size(800,600,P3D);

}
void draw()
{

  // Change projective matrix to orthographic
  ortho(-width/2, width/2, -height/2, height/2, -1000, 1000);

  background(255);
  translate(width/2, height/2, 0);
  translate(0.01,0.01,0.01);
  rotateX(0.18);
  rotateY(0.11);
  rotateZ(0.00);
  
  scale(133);  
 // noStroke();
  strokeWeight(1);

  // Represent three-dimensional axes
  stroke(255, 0, 0); // Set stroke color for X axis
  pushMatrix();
  scale(0.01);
  line(0,0,0,84,0,0);
  fill(255, 0, 0);
  text("X", 75, -5, 0);
  popMatrix();
  
  stroke(0, 255, 0); // Set stroke color for Y axis
  pushMatrix();
  scale(0.01);
  line(0,0,0,0,100,0);
  fill(0, 255, 0);
  text("Y", 0, 105, 0);
  popMatrix();
  
  stroke(0, 0, 255); // Set stroke color for Z axis
  pushMatrix();
  scale(0.01);
  line(0,0,0,0,0,100);
  fill(0, 0, 255);
  text("Z", 0, 0, 105);
  popMatrix();

  // Represent room geometry, paint only first wall
  noStroke();
  fill(87, 87, 90);
  beginShape();
  vertex(2,2,-1);
  vertex(2,-2,-1);
  vertex(2,-2,1);
  vertex(2,2,1);
  endShape(CLOSE);
  pushMatrix();
  scale(0.01);
  text("Wall: ", 114, -134, 0);
  text("2,2,-1", 114, -124, 0);
  text("2,-2,-1", 114, -114, 0);
  text("2,-2,1", 114, -104, 0);
  text("2,2,1", 114, -94, 0);
  popMatrix();
  
  // Represent listener location including text laber 'Listener'
  fill(90, 87, 87);
  pushMatrix();
  translate(-0.45, 0.02, -0.68);
  box(0.1, 0.1, 0.1);
  pushMatrix();
  scale(0.01);
  text("Listener", 0.06, 22, 0);
  text("-0.45, 0.02, -0.68", 0.06, 34, 0);
  popMatrix();
  popMatrix();

  // Represent source location including text laber 'Source'
  fill(87, 90, 87);
  pushMatrix();
  translate(1, 0, 0);
  sphere(0.1);
  pushMatrix();
  scale(0.01);
  text("Source", 1.5, 28, 0);
  text("1, 0, 0", 1.5, 38, 0);
  popMatrix();
  popMatrix();


 
}
