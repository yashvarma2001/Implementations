public class TokenBucketRateLimiter {
    private final long capacity;
    private final double refillRatePerSecond;
    private double currentTokens;
    private long lastRefillTimestamp;

    public TokenBucketRateLimiter(long capacity, double refillRatePerSecond) {
        this.capacity = capacity;
        this.refillRatePerSecond = refillRatePerSecond;
        this.currentTokens = capacity;
        this.lastRefillTimestamp = System.currentTimeMillis();
    }

    // CONCEPT 1: Synchronized for Thread Safety
    public synchronized boolean allowRequest(int tokensNeeded) {
        long now = System.currentTimeMillis();
        
        // CONCEPT 2: Lazy Refill Math
        // Calculate seconds passed (Use double for precision)
        double timePassedSeconds = (now - this.lastRefillTimestamp) / 1000.0;
        
        // Calculate tokens to add
        double tokensToAdd = timePassedSeconds * this.refillRatePerSecond;
        
        // Update bucket (Refill, but don't overflow capacity)
        if (tokensToAdd > 0) {
            this.currentTokens = Math.min(this.capacity, this.currentTokens + tokensToAdd);
            this.lastRefillTimestamp = now; 
        }

        // CHECK: Do we have enough?
        if (this.currentTokens >= tokensNeeded) {
            this.currentTokens -= tokensNeeded;
            return true;
        } else {
            return false;
        }
    }
    
    public static void main(String[] args) throws InterruptedException {
        // Setup: Bucket holds 10 tokens, refills 1 token per second
        TokenBucketRateLimiter limiter = new TokenBucketRateLimiter(10, 1);
        System.out.println("--- Starting Test ---");

        // Test 1: Immediate consumption
        // Should PASS (We start with 10 tokens)
        boolean result1 = limiter.allowRequest(10);
        System.out.println("Request 1 (Take 10): " + (result1 ? "PASS ✅" : "FAIL ❌"));

        // Test 2: Immediate Fail
        // Should FAIL (Bucket is empty, 0 tokens left)
        boolean result2 = limiter.allowRequest(1);
        System.out.println("Request 2 (Take 1):  " + (!result2 ? "PASS ✅ (Correctly rejected)" : "FAIL ❌"));

        // Test 3: Wait for Refill
        System.out.println("Sleeping for 2 seconds to refill...");
        Thread.sleep(2000); // Wait 2 seconds

        // Should PASS (We refilled ~2 tokens)
        boolean result3 = limiter.allowRequest(1);
        System.out.println("Request 3 (Take 1):  " + (result3 ? "PASS ✅" : "FAIL ❌"));
    }
}