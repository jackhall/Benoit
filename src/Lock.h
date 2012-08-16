#ifndef BenoitLock_h
#define BenoitLock_h

/*
    Benoit: a flexible framework for distributed graphs
    Copyright (C) 2011  Jack Hall

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    e-mail: jackwhall7@gmail.com
*/

#include <atomic>
#include <mutex>

namespace ben {

	class ScopedReadLock;
	class ScopedWriteLock; 

	class Commons {
	/*
		Implements write once, read many semantics. Do not call
		recursively from one thread! Each thread should only lock 
		once at a time. 
		
		This type of lock is also called readers-writer or shared-exclusive
	*/
	private:
		std::atomic<short> num_reads;
		std::atomic<bool> reading;
		std::mutex write_mutex;
		
	public:
		Commons() : num_reads(0), reading(true) {}
		Commons(const Commons& rhs) = delete;
		Commons& operator=(const Commons& rhs) = delete;
	
		void read_lock() {
			while( !reading.load() ) {}
			num_reads.fetch_add(1);
		}
		
		bool try_read_lock() {
			if( reading.load() ) {
				num_reads.fetch_add(1);
				return true;
			} else  return false;
		}
		
		void read_unlock() {
			if(num_reads.fetch_sub(1) < 0) 
				num_reads.fetch_add(1);
		}
		
		void write_lock() {
			write_mutex.lock();
			reading.store(false); 
			//hangs if read_lock is being called recursively from another thread
			while(num_reads.load() > 0) {}
		}
		
		bool try_write_lock() {
			if( write_mutex.try_lock() ) {
				reading.store(false);
				if( num_reads > 0 ) {
					reading.store(true);
					write_mutex.unlock();
					return false; //someone else is reading
				} else return true; 
			} else return false; //someone else is writing
		}
		
		void write_unlock() {
			reading.store(true);
			write_mutex.unlock();
		}
		
	}; //class Commons

	
	class ScopedReadLock {
	/*
		Similar to std::lock_guard. Acquires read lock on construction
		and guarantees its release when destructed. Do not create more than
		one of these per thread, even though it might let you sometimes!
	*/
	private:
		bool locked;
		Commons* commons;
		
	public:
		ScopedReadLock() = delete;
		explicit ScopedReadLock(Commons& resource)
			: locked(true), commons(&resource) {
			commons->read_lock();
		}
		ScopedReadLock(const ScopedReadLock& rhs) = delete; 
		ScopedReadLock& operator=(const ScopedReadLock& rhs) = delete;
		operator bool const { return locked; }
		~ScopedReadLock() { unlock(); }
		
		void unlock() {
			if(locked) {
				locked = false;
				commons->read_unlock;
			}
		}
	}; //class ScopedReadLock

	
	class ScopedWriteLock {
	/*
		Similar to std::lock_guard. Acquires write lock on construction
		and guarantees its release when destructed. Will hang if called 
		more than once in the same thread. 
		
		Downside to RAII-controlled exclusive locks: they are released 
		early if an exception is thrown in the middle of a write, exposing
		an invalid state to reader code. 
	*/
	private:
		bool locked;
		Commons* commons;
	
	public:
		ScopedWriteLock() = delete;
		explicit ScopedWriteLock(Commons& commons) 
			: locked(true), commons(&resource) {
			commons->write_lock();
		}
		ScopedWriteLock(const ScopedWriteLock& rhs) = delete; 
		ScopedWriteLock& operator=(const ScopedWriteLock& rhs) = delete;
		operator bool const { return locked; }
		~ScopedWriteLock() { unlock(); }
		
		void unlock() {
			if(locked) {
				locked = false;
				commons->write_unlock;
			}
		}
	}; //class ScopedWriteLock

} //namespace ben

#endif

