package stabilitymodel;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ManageFiles {

    int visit = 0;
    Boolean recursive;

    public ManageFiles() {

    }

    public Vector readFile(String nome_arq) {

        GraphModel g = new GraphModel();
        Vector v = new Vector();

        try {

            BufferedReader in;
            in = new BufferedReader(new InputStreamReader(new FileInputStream(nome_arq), "ISO-8859-1"));

            String str;
            String temp;
            boolean cont = true;
            while (in.ready()) {
                str = in.readLine();
                if (cont != true) {
                    StringTokenizer st = new StringTokenizer(str, " ");
                    g = new GraphModel();
                    int parameter = 1;
                    while (st.hasMoreTokens()) {
                        temp = st.nextToken();
                        if (parameter == 1) {
                            g.num_step = Integer.parseInt(temp);
                        }
                        else if (parameter == 2) {
                            g.num_message = Integer.parseInt(temp);
                        }
                        else if (parameter == 3) {
                            g.num_state = Integer.parseInt(temp);
                        }
                        else if (parameter == 4) {
                            g.num_infected = Integer.parseInt(temp);
                        }
                        parameter++;
                    }
                    v.add(g);
                }
                cont = false;
            }

            in.close();
            return v;

        } catch (UnsupportedEncodingException ex) {
            Logger.getLogger(ManageFiles.class.getName()).log(Level.SEVERE, null, ex);
        } catch (FileNotFoundException ex) {
            Logger.getLogger(ManageFiles.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(ManageFiles.class.getName()).log(Level.SEVERE, null, ex);
        }

        return v;
    }

}
