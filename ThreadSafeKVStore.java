import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class ThreadSafeKVStore {
    private final Map<String, String> store;
    private final ReadWriteLock rwLock;
    private final Lock readLock;
    private final Lock writeLock;

    public ThreadSafeKVStore() {
        this.store = new HashMap<>();
        // "true" enables Fairness (prevents Writer Starvation if readers are endless)
        this.rwLock = new ReentrantReadWriteLock(true); 
        this.readLock = rwLock.readLock();
        this.writeLock = rwLock.writeLock();
    }

    /**
     * Tries to put a value, but gives up if the lock is busy for too long.
     * Prevents system hang.
     */
    public boolean tryPut(String key, String value) throws InterruptedException {
        // Feature 1: tryLock with Timeout (Don't wait forever!)
        if (writeLock.tryLock(2, TimeUnit.SECONDS)) {
            try {
                store.put(key, value);
                return true;
            } finally {
                writeLock.unlock();
            }
        } else {
            System.err.println("⚠️ Write Lock acquisition timed out! System is busy.");
            return false;
        }
    }

    /**
     * Classic Atomic Check-Then-Act.
     * We must hold the Write lock during the 'contains' check AND the 'put'.
     */
    public boolean putIfAbsent(String key, String value) {
        writeLock.lock(); // Exclusive lock
        try {
            if (!store.containsKey(key)) {
                store.put(key, value);
                return true;
            }
            return false;
        } finally {
            writeLock.unlock();
        }
    }

    public String get(String key) {
        readLock.lock();
        try {
            // Simulation: Making reads "slow" to prove multiple readers can enter at once
            try { Thread.sleep(50); } catch (InterruptedException e) {} 
            return store.get(key);
        } finally {
            readLock.unlock();
        }
    }

    // --- ROBUST TEST HARNESS ---
    public static void main(String[] args) throws InterruptedException {
        ThreadSafeKVStore kvStore = new ThreadSafeKVStore();

        // 1. READER GROUP: 5 threads reading simultaneously
        // This proves the "Shared" nature of ReadLock. 
        // If this was 'synchronized', they would run one by one (taking 250ms total).
        // With ReadLock, they run in parallel (taking ~50ms total).
        Runnable readerTask = () -> {
            long start = System.currentTimeMillis();
            kvStore.get("config");
            System.out.println("📖 Reader finished in: " + (System.currentTimeMillis() - start) + "ms");
        };

        System.out.println("--- Starting Parallel Read Test ---");
        for (int i = 0; i < 5; i++) {
            new Thread(readerTask).start();
        }
        
        Thread.sleep(1000); // Wait for readers to finish

        // 2. TIMEOUT TEST: Simulate a deadlock/busy writer
        System.out.println("\n--- Starting Timeout Test ---");
        
        // Thread A: Holds the write lock forever (The "Bug")
        new Thread(() -> {
            kvStore.rwLock.writeLock().lock();
            System.out.println("🔒 Thread A acquired Write Lock (and won't release it)");
            try { Thread.sleep(5000); } catch (InterruptedException e) {} 
            // Forgot to unlock! (Simulating crash/hang)
        }).start();

        Thread.sleep(100); 

        // Thread B: Tries to write, but should fail gracefully instead of hanging
        new Thread(() -> {
            System.out.println("Thread B trying to acquire lock...");
            try {
                boolean success = kvStore.tryPut("key", "val");
                System.out.println("Thread B result: " + (success ? "Success" : "Failed (Timeout works!) ✅"));
            } catch (InterruptedException e) { e.printStackTrace(); }
        }).start();
    }
}