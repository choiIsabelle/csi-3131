/*
Last Names: Casey, Choi
Student Numbers: 300291133, 3003282805
-the program will generate a random 15-digit reference string with up to 10 different pages
-the program will choose to use 1-7 frames for the memory
-the program will then run the FIFO and LRU page replacement algorithms
-using that frame number and that reference string
-and record the number of page faults

usage: java q2
*/

import java.util.Random;

public class q2 {
    public static void main(String[] args){
        Random r = new Random();

        //generate reference string
        int[] referencestring = new int[15];
        for(int i=0; i<15; i++){
            referencestring[i] = r.nextInt(10);
        }

        //print reference string
        System.out.print("The random reference string is: ");
        for(int i = 0; i<referencestring.length; i++){
            System.out.print(referencestring[i]+" ");
        }
        System.out.println();

        //generate number of frames
        int numberofframes = r.nextInt(7) + 1;
        System.out.println("The number of frames in memory is: " + numberofframes);

        //run fifo
        int faultsFIFO = FIFO(numberofframes, referencestring);
        System.out.println("The FIFO page replacement algorithm had " + faultsFIFO + " page faults");

        //run fifo
        int faultsLRU = LRU(numberofframes, referencestring);
        System.out.println("The LRU page replacement algorithm had " + faultsLRU + " page faults");
    }

    public static PageFrame[] createMemory(int frames){ //create memory
        PageFrame[] memory = new PageFrame[frames];
        for (int i=0; i<frames; i++){
            memory[i] = new PageFrame(i);
        }
        return memory;
    }

    public static void printMemory(PageFrame[] mem, int time){ //printing method for debugging
        System.out.print("At time " + time + " the memory looks like: ");
        int page;
        for (int i=0; i<mem.length; i++){
            page = mem[i].getPageNo();
            if (page == -1){System.out.print("X ");}
            else {System.out.print(page + " ");}
        }
        System.out.println();
    }

    public static int checkInMemory(PageFrame[] mem, int page){//checks if the page is already loaded in memory, returns index if true, -1 if not
        for (int i = 0; i < mem.length; i++){
            if (mem[i].getPageNo() == page){
                return i;
            }
        }
        return -1;
    }

    public static int firstEmptyFrame(PageFrame[] mem){ //returns the index of the first empty frame in mem, if mem is full it returns -1
        for (int i = 0; i<mem.length; i++){
            if(mem[i].getPageNo() == -1){
                return i;
            }
        }
        return -1;
    }

    public static int selectFIFO(PageFrame[] mem){ //selects victim page according to FIFO algorithm
        int oldestpage = 0;
        int oldestpagetime = mem[0].getWhenLoaded();
        int temp;
        for (int i=1; i<mem.length; i++){
            temp = mem[i].getWhenLoaded();
            if (temp < oldestpagetime){//if ith page is older
                oldestpage = i;
                oldestpagetime = temp;
            }
        }
        return oldestpage;
    }

    public static int selectLRU(PageFrame[] mem){ //selects victim page according to LRU algorithm
        int referencedpage = 0;
        int oldestreference = mem[0].getLastReference();
        int temp;
        for (int i=1; i<mem.length; i++){
            temp = mem[i].getLastReference();
            if (temp < oldestreference){//if ith page is older
                referencedpage = i;
                oldestreference = temp;
            }
        }
        return referencedpage;
    }

    public static int FIFO(int framenumber, int[] refstr){//method for fifo, returns number of page faults encountered
        int faultcount = 0;
        int time = 0;
        int refpage;
        int victimframe;
        PageFrame[] mem = createMemory(framenumber);

        for (int i=0; i<refstr.length; i++){
            refpage = refstr[i];
            if (checkInMemory(mem, refpage) == -1){ //if the page isn't already in memory, page fault occurs
                faultcount++; //increase page fault count
                victimframe = firstEmptyFrame(mem); //look for first empty frame to be victim
                if (victimframe == -1){victimframe = selectFIFO(mem);} //if memory is full, use FIFO criteria to find the victim
                mem[victimframe].swapPage(refpage, time); //swap
            }
            //for visualization, print
            //printMemory(mem, time);

            time++; //increase time counter
        }
        return faultcount;
    }

    public static int LRU(int framenumber, int[] refstr){//method for lru, returns number of page faults encountered
        int faultcount = 0;
        int time = 0;
        int refpage;
        int victimframe;
        int inmemory;
        PageFrame[] mem = createMemory(framenumber);

        for (int i=0; i<refstr.length; i++){
            refpage = refstr[i];
            inmemory = checkInMemory(mem, refpage);
            if (inmemory== -1){ //if the page isn't already in memory, page fault occurs
                faultcount++; //increase page fault count
                victimframe = firstEmptyFrame(mem); //look for first empty frame to be victim
                if (victimframe == -1){victimframe = selectLRU(mem);} //if memory is full, use FIFO criteria to find the victim
                mem[victimframe].swapPage(refpage, time); //swap
            }
            else { //if the page is in memory, update the page's reference time
                mem[inmemory].updateReference(time);
            }
            //for visualization, print
            //printMemory(mem, time);
            time++; //increase time counter
        }
        return faultcount;
    }
}



class PageFrame{
    int frameno; //the frame number (for printing)
    int pageno; //stores the page number loaded into the frame
    int timeloaded; //stores the time the page was loaded into the frame
    int timelastreference; //stores the last time the page in the frame was referenced

    public PageFrame(int frame){ //constructor method
        this.frameno = frame;
        this.pageno = -1;
        this.timeloaded = -1;
        this.timelastreference = -1;
    }

    public void swapPage(int page, int time){ //changes the page stored in the frame and updates time values
        this.pageno = page;
        updateReference(time);
        updateTimeLoaded(time);
    }

    public void updateTimeLoaded(int time){//updates the loading time 
        this.timeloaded = time;
    }

    public void updateReference(int time){ // updates the last reference time
        this.timelastreference = time;
    }

    public int getWhenLoaded(){ //checks the loading time
        return this.timeloaded;
    }

    public int getLastReference(){ //checks the last reference time
        return this.timelastreference;
    }

    public int getPageNo(){
        return this.pageno;
    }

    public int getFrameNo(){
        return this.frameno;
    }

    public String toString(){ //for printing
        return "Frame " + this.frameno + ", storing page " + this.pageno + " since " + this.timeloaded + " which was last reference at " + timelastreference;
    }

}