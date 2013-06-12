/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package stabilitymodel;

import java.util.Vector;

/**
 *
 * @author Daniel
 */
public class StabilityModel {

    public static void main(String[] args) {

        Vector graphs;

        ManageFiles mf = new ManageFiles();
        graphs = mf.readFile("testesis.txt");
        System.out.println(graphs.size());

        double values = 0;
        int iters = 0;
        for (int i = 0; i < graphs.size(); i++) {
            for (int j = 0; j < graphs.size(); j++) {
                if (i != j) {
                    GraphModel gi = (GraphModel) graphs.get(i);
                    GraphModel gj = (GraphModel) graphs.get(j);

                    double value = euclideanDistance(gi.num_step, gi.num_infected, gj.num_step, gj.num_infected);
                    values += value;
                    //System.out.println(value);
                    iters++;
                }
            }
        }
        System.out.println(values/iters);
    }

    static double euclideanDistance(Integer ax1, Integer ay1, Integer ax2, Integer ay2) {
        double x1 = new Double(ax1);
        double y1 = new Double(ay1);
        double x2 = new Double(ax2);
        double y2 = new Double(ay2);

        double xDiff = x1 - x2;
        double xSqr = Math.pow(xDiff, 2);

        double yDiff = y1 - y2;
        double ySqr = Math.pow(yDiff, 2);

        double distance = Math.sqrt(xSqr + ySqr);
        
        return distance;
    }
}
