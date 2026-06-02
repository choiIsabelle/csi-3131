/**
 * Worker thread that fills a shared array with Fibonacci numbers.
 */
public class FibonacciGenerator implements Runnable {
    private final long[] sequence;

    // constructor
    public FibonacciGenerator(long[] sequence) {
        this.sequence = sequence;
    }

    @Override
    public void run() {
        int n = sequence.length;
        if (n == 0) {
            return;
        }
        sequence[0] = 0;
        if (n == 1) {
            return;
        }
        sequence[1] = 1;
        for (int i = 2; i < n; i++) {
            sequence[i] = sequence[i - 1] + sequence[i - 2];
        }
    }
}
