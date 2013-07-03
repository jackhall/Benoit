#ifndef BenoitCommons_h
#define BenoitCommons_h

/*
    Benoit: a distributed graphs library
    Copyright (C) 2012  Jack Hall

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
#include <condition_variable>
#include <iostream>
#include <boost/thread/shared_mutex.hpp>

namespace ben {

	class Commons {
	/*
		Implements write once, read many semantics. Do not call
		recursively from one thread! Each thread should only lock 
		once at a time. 
		
		This type of lock is also called readers-writer or shared-exclusive.
		
		This particular version of Commons simply wraps boost:shared_mutex.
	*/
	private:
		boost::shared_mutex rwlock;
		
	public:
		Commons() = default;
		Commons(const Commons& rhs) = delete;
		Commons& operator=(const Commons& rhs) = delete;
		virtual ~Commons() = default;
		
		//observers
		//bool read_locked() const { return readers.load() > 0; }
		//bool writer_waiting() const { return waiting_writers.load() > 0; }
		//bool write_locked() {
		//	if(rwlock.try_lock()) {
		//		rwlock.unlock();
		//		return false;
		//	} else return true;
		//}
		
		//read or shared locking
		void read_lock() { rwlock.lock_shared(); }
		bool try_read_lock() { rwlock.try_lock_shared(); }
		void read_unlock() { rwlock.unlock_shared(); }
		
		//write or unique locking
		void write_lock() { rwlock.lock(); }
		bool try_write_lock() { rwlock.try_lock(); }
		void write_unlock() { rwlock.unlock(); }
		
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
		operator bool() const { return locked; }
		~ScopedReadLock() { unlock(); }
		
		void unlock() {
			if(locked) {
				locked = false;
				commons->read_unlock();
			}
		}
	}; //class ScopedReadLock

	
	class ScopedWriteLock {
	/*
		Similar to std::lock_guard. Acquires write lock on construction
		and guarantees its release when destructed. Will hang if constructed 
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
		explicit ScopedWriteLock(Commons& resource) 
			: locked(true), commons(&resource) {
			commons->write_lock();
		}
		ScopedWriteLock(const ScopedWriteLock& rhs) = delete; 
		ScopedWriteLock& operator=(const ScopedWriteLock& rhs) = delete;
		operator bool() const { return locked; }
		~ScopedWriteLock() { unlock(); }
		
		void unlock() {
			if(locked) {
				locked = false;
				commons->write_unlock();
			}
		}
	}; //class ScopedWriteLock

} //namespace ben

#endif

