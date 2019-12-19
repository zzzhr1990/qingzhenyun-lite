//
// Created by herui on 2019/12/16.
//

#ifndef QINGZHENYUN_LITE_BASIC_FILE_BUF_H
#define QINGZHENYUN_LITE_BASIC_FILE_BUF_H

#include <fstream>
#include <cpprest/filestream.h>
#include <atomic>

namespace qingzhen::transfer {
    template<typename _CharType>
    class basic_file_buf : public concurrency::streams::details::basic_streambuf<_CharType> {
		typedef typename concurrency::streams::char_traits<_CharType> _traits;
        typedef typename concurrency::streams::details::basic_streambuf<_CharType>::int_type int_type;
        typedef typename concurrency::streams::details::basic_streambuf<_CharType>::pos_type pos_type;
        typedef typename concurrency::streams::details::basic_streambuf<_CharType>::off_type off_type;
    public:
        /*
        basic_file_buf(const std::filesystem::path& path, const pos_type pos, const std::ios_base& open_mode) {
            this->open_file(path, pos, open_mode);
        };
         */
        basic_file_buf() = default;

        ~basic_file_buf() {
            if (_is_open) {
                this->buf.close();
            }
        }

        // std::ios::out | std::ios::binary
        bool open_file(const std::filesystem::path &path, const pos_type pos, std::ios_base::openmode open_mode) {
            try {
                //std::fstream fs;
                //fs.op
#ifndef _WIN32
                _is_open = this->buf.open(path, open_mode)->is_open();
#else
                _is_open = this->buf.open(path, open_mode, std::ios_base::_Openprot);
#endif
                // _is_open = true;
                // std::cout << "IS_OPEN_TRUE" << std::endl;
                if (pos >= 0) {
                    current_pos = buf.pubseekpos(pos, std::ios_base::out);
                }
                // std::cout << "file: " << path.c_str() << " seek to:" << current_pos << std::endl;
                return true;
            } catch (const std::exception &eee) {
                std::cout << "onen file err: " << eee.what() << std::endl;
            }
            return false;
        }

        void set_file_can_write(bool can_write) {
			std::lock_guard<std::mutex> _mu(this->_buffer_mutex);
			for (auto& ch : this->_buffer) {
				this->buf.sputc(ch);
				current_pos += 1;
			}
            _can_write = can_write;
        }

        /// <summary>
        /// <c>can_read</c> is used to determine whether a stream buffer will support read operations (get).
        /// </summary>
        [[nodiscard]] bool can_read() const override {
            std::cout << "can_read()" << std::endl;
            return true;
        };

        /// <summary>
        /// <c>can_write</c> is used to determine whether a stream buffer will support write operations (put).
        /// </summary>
        [[nodiscard]] bool can_write() const override {
            return _is_open;
        };

        /// <summary>
        /// <c>can_seek<c/> is used to determine whether a stream buffer supports seeking.
        /// </summary>
        [[nodiscard]] bool can_seek() const override {
            std::cout << "can_seek" << std::endl;
            return true;
        };

        /// <summary>
        /// <c>has_size<c/> is used to determine whether a stream buffer supports size().
        /// </summary>
        [[nodiscard]]  bool has_size() const override {
            std::cout << "has_size" << std::endl;
            return true;
        };

        /// <summary>
        /// <c>is_eof</c> is used to determine whether a read head has reached the end of the buffer.
        /// </summary>
        [[nodiscard]] bool is_eof() const override {
            std::cout << "is_eof" << std::endl;
            return true;
        };

        /// <summary>
        /// Gets the stream buffer size, if one has been set.
        /// </summary>
        /// <param name="direction">The direction of buffering (in or out)</param>
        /// <returns>The size of the internal buffer (for the given direction).</returns>
        /// <remarks>An implementation that does not support buffering will always return 0.</remarks>
        [[nodiscard]] size_t buffer_size(std::ios_base::openmode direction) const override {
            // std::ios_base::in default
            std::cout << "buffer_size" << std::endl;
            return 0;
        };

        /// <summary>
        /// Sets the stream buffer implementation to buffer or not buffer.
        /// </summary>
        /// <param name="size">The size to use for internal buffering, 0 if no buffering should be done.</param>
        /// <param name="direction">The direction of buffering (in or out)</param>
        /// <remarks>An implementation that does not support buffering will silently ignore calls to this function and it
        /// will not have any effect on what is returned by subsequent calls to <see cref="::buffer_size method"
        /// />.</remarks>
        void set_buffer_size(size_t size, std::ios_base::openmode direction) override {
            //  = std::ios_base::in dir
            std::cout << "set_buffer_size" << std::endl;
        };

        /// <summary>
        /// For any input stream, <c>in_avail</c> returns the number of characters that are immediately available
        /// to be consumed without blocking. May be used in conjunction with <cref="::sbumpc method"/> to read data without
        /// incurring the overhead of using tasks.
        /// </summary>
        [[nodiscard]] size_t in_avail() const override {
            std::cout << "in_avail" << std::endl;
            return 1024;
        };

        /// <summary>
        /// Checks if the stream buffer is open.
        /// </summary>
        /// <remarks>No separation is made between open for reading and open for writing.</remarks>
        [[nodiscard]] bool is_open() const override {
            return _is_open;
        };

        /// <summary>
        /// Closes the stream buffer, preventing further read or write operations.
        /// </summary>
        /// <param name="mode">The I/O mode (in or out) to close for.</param>
        pplx::task<void> close(std::ios_base::openmode mode) override {
            // = (std::ios_base::in | std::ios_base::out) default func
            if (this->_is_open) {
				this->buf.pubsync();
				this->buf.close();
                this->_is_open = false;
            }
            return pplx::task_from_result();
        };

        /// <summary>
        /// Closes the stream buffer with an exception.
        /// </summary>
        /// <param name="mode">The I/O mode (in or out) to close for.</param>
        /// <param name="eptr">Pointer to the exception.</param>
        pplx::task<void> close(std::ios_base::openmode mode, std::exception_ptr exceptionPtr) override {
            if (this->_is_open) {
				this->buf.pubsync();
                this->buf.close();
                this->_is_open = false;
            }
            return pplx::task_from_result();
        };

        /// <summary>
        /// Writes a single character to the stream.
        /// </summary>
        /// <param name="ch">The character to write</param>
        /// <returns>A <c>task</c> that holds the value of the character. This value is EOF if the write operation
        /// fails.</returns>
        pplx::task<int_type> putc(_CharType ch) override {
            if (this->_can_write) {
                // need do it async?
                auto s = this->buf.sputc(ch);
                current_pos += s;
                return pplx::task_from_result<int_type>(s);

            }
            return pplx::task_from_result<int_type>(0);
        };

        /// <summary>
        /// Writes a number of characters to the stream.
        /// </summary>
        /// <param name="ptr">A pointer to the block of data to be written.</param>
        /// <param name="count">The number of characters to write.</param>
        /// <returns>A <c>task</c> that holds the number of characters actually written, either 'count' or 0.</returns>
        pplx::task<size_t> putn(const _CharType *ptr, size_t count) override {
			std::cout << "_putn:" << count << std::endl;
            if (this->_can_write) {
                // need do it async?
                auto s = this->buf.sputn(ptr, count);
                current_pos += s;
                return pplx::task_from_result<size_t>(static_cast<size_t>(s));
            } 
            return pplx::task_from_result<size_t>(0);
        };

        /// <summary>
        /// Writes a number of characters to the stream. Note: callers must make sure the data to be written is valid until
        /// the returned task completes.
        /// </summary>
        /// <param name="ptr">A pointer to the block of data to be written.</param>
        /// <param name="count">The number of characters to write.</param>
        /// <returns>A <c>task</c> that holds the number of characters actually written, either 'count' or 0.</returns>
        pplx::task<size_t> putn_nocopy(const _CharType *ptr, size_t count) override {
			std::lock_guard<std::mutex> _mu(this->_buffer_mutex);
			if (this->_can_write) {
                // need do it async?
                // hdd / ssd can finish write in few millisecond, so async call is unnecessary.
                pos_type s = this->buf.sputn(ptr, count);
                current_pos += s;
                return pplx::task_from_result<size_t>(static_cast<size_t>(s));
			}
			else {
				// std::cout << "____________BUFFB" << std::endl;
				for (size_t i = 0; i < count; i++) {
					this->_buffer.push_back(ptr[i]);
				}
				
			}

            return pplx::task<size_t>([count]() {
                return count;
            });
        };

        /// <summary>
        /// Reads a single character from the stream and advances the read position.
        /// </summary>
        /// <returns>A <c>task</c> that holds the value of the character. This value is EOF if the read fails.</returns>
        pplx::task<int_type> bumpc() override {
            std::cout << "bumpc" << std::endl;
            return pplx::task_from_result<int_type>(0);
        };

        /// <summary>
        /// Reads a single character from the stream and advances the read position.
        /// </summary>
        /// <returns>The value of the character. <c>-1</c> if the read fails. <c>-2</c> if an asynchronous read is
        /// required</returns> <remarks>This is a synchronous operation, but is guaranteed to never block.</remarks>
        int_type sbumpc() override {
            std::cout << "sbumpc" << std::endl;
            return 0;
        };

        /// <summary>
        /// Reads a single character from the stream without advancing the read position.
        /// </summary>
        /// <returns>A <c>task</c> that holds the value of the byte. This value is EOF if the read fails.</returns>
        pplx::task<int_type> getc() override {
            std::cout << "getc" << std::endl;
            return pplx::task_from_result<int_type>(0);
        };

        /// <summary>
        /// Reads a single character from the stream without advancing the read position.
        /// </summary>
        /// <returns>The value of the character. EOF if the read fails. <see cref="::requires_async method" /> if an
        /// asynchronous read is required</returns> <remarks>This is a synchronous operation, but is guaranteed to never
        /// block.</remarks>
        int_type sgetc() override {
            std::cout << "sgetc" << std::endl;
            return 0;
        };

        /// <summary>
        /// Advances the read position, then returns the next character without advancing again.
        /// </summary>
        /// <returns>A <c>task</c> that holds the value of the character. This value is EOF if the read fails.</returns>
        pplx::task<int_type> nextc() override {
            std::cout << "nextc" << std::endl;
            return pplx::task_from_result<int_type>(0);
        };

        /// <summary>
        /// Retreats the read position, then returns the current character without advancing.
        /// </summary>
        /// <returns>A <c>task</c> that holds the value of the character. This value is EOF if the read fails,
        /// <c>requires_async</c> if an asynchronous read is required</returns>
        pplx::task<int_type> ungetc() override {
            std::cout << "ungetc" << std::endl;
            return pplx::task_from_result<int_type>(0);
        };

        /// <summary>
        /// Reads up to a given number of characters from the stream.
        /// </summary>
        /// <param name="ptr">The address of the target memory area.</param>
        /// <param name="count">The maximum number of characters to read.</param>
        /// <returns>A <c>task</c> that holds the number of characters read. This value is O if the end of the stream is
        /// reached.</returns>
        pplx::task<size_t> getn(_Out_writes_(count) _CharType *ptr, _In_ size_t count) override {
            std::cout << "getn" << std::endl;
            return pplx::task_from_result<size_t>(0);
        };

        /// <summary>
        /// Copies up to a given number of characters from the stream, synchronously.
        /// </summary>
        /// <param name="ptr">The address of the target memory area.</param>
        /// <param name="count">The maximum number of characters to read.</param>
        /// <returns>The number of characters copied. O if the end of the stream is reached or an asynchronous read is
        /// required.</returns> <remarks>This is a synchronous operation, but is guaranteed to never block.</remarks>
        size_t scopy(_Out_writes_(count) _CharType *ptr, _In_ size_t count) override {
            std::cout << "scopy" << std::endl;
            return 0;
        };

        /// <summary>
        /// Gets the current read or write position in the stream.
        /// </summary>
        /// <param name="direction">The I/O direction to seek (see remarks)</param>
        /// <returns>The current position. EOF if the operation fails.</returns>
        /// <remarks>Some streams may have separate write and read cursors.
        ///          For such streams, the direction parameter defines whether to move the read or the write
        ///          cursor.</remarks>
        pos_type getpos(std::ios_base::openmode direction) const override {
            return current_pos;
        };

        /// <summary>
        /// Gets the size of the stream, if known. Calls to <c>has_size</c> will determine whether
        /// the result of <c>size</c> can be relied on.
        /// </summary>
        [[nodiscard]] utility::size64_t size() const override {
            std::cout << "size" << std::endl;
            return 1024;
        };

        /// <summary>
        /// Seeks to the given position.
        /// </summary>
        /// <param name="pos">The offset from the beginning of the stream.</param>
        /// <param name="direction">The I/O direction to seek (see remarks).</param>
        /// <returns>The position. EOF if the operation fails.</returns>
        /// <remarks>Some streams may have separate write and read cursors. For such streams, the direction parameter
        /// defines whether to move the read or the write cursor.</remarks>
        pos_type seekpos(pos_type pos, std::ios_base::openmode direction) override {
            std::cout << "seekpos" << std::endl;
            return 1024;
        };

        /// <summary>
        /// Seeks to a position given by a relative offset.
        /// </summary>
        /// <param name="offset">The relative position to seek to</param>
        /// <param name="way">The starting point (beginning, end, current) for the seek.</param>
        /// <param name="mode">The I/O direction to seek (see remarks)</param>
        /// <returns>The position. EOF if the operation fails.</returns>
        /// <remarks>Some streams may have separate write and read cursors.
        ///          For such streams, the mode parameter defines whether to move the read or the write cursor.</remarks>
        pos_type seekoff(off_type offset, std::ios_base::seekdir way, std::ios_base::openmode mode) override {
            std::cout << "seekoff" << std::endl;
            return 1024;
        };

        /// <summary>
        /// For output streams, flush any internally buffered data to the underlying medium.
        /// </summary>
        /// <returns>A <c>task</c> that returns <c>true</c> if the sync succeeds, <c>false</c> if not.</returns>
        pplx::task<void> sync() override {
            std::cout << "sync" << std::endl;
            return pplx::task_from_result();
        };

        //
        // Efficient read and write.
        //
        // The following routines are intended to be used for more efficient, copy-free, reading and
        // writing of data from/to the stream. Rather than having the caller provide a buffer into which
        // data is written or from which it is read, the stream buffer provides a pointer directly to the
        // internal data blocks that it is using. Since not all stream buffers use internal data structures
        // to copy data, the functions may not be supported by all. An application that wishes to use this
        // functionality should therefore first try them and check for failure to support. If there is
        // such failure, the application should fall back on the copying interfaces (putn / getn)
        //

        /// <summary>
        /// Allocates a contiguous memory block and returns it.
        /// </summary>
        /// <param name="count">The number of characters to allocate.</param>
        /// <returns>A pointer to a block to write to, null if the stream buffer implementation does not support
        /// alloc/commit.</returns>
        _CharType *alloc(_In_ size_t count) override {
            // std::cout << "alloc size:" << count << std::endl;
            return nullptr;
        };

        /// <summary>
        /// Submits a block already allocated by the stream buffer.
        /// </summary>
        /// <param name="count">The number of characters to be committed.</param>
        void commit(_In_ size_t count) override {
            std::cout << "commit: " << count << std::endl;
        };

        /// <summary>
        /// Gets a pointer to the next already allocated contiguous block of data.
        /// </summary>
        /// <param name="ptr">A reference to a pointer variable that will hold the address of the block on success.</param>
        /// <param name="count">The number of contiguous characters available at the address in 'ptr'.</param>
        /// <returns><c>true</c> if the operation succeeded, <c>false</c> otherwise.</returns>
        /// <remarks>
        /// A return of false does not necessarily indicate that a subsequent read operation would fail, only that
        /// there is no block to return immediately or that the stream buffer does not support the operation.
        /// The stream buffer may not de-allocate the block until <see cref="::release method" /> is called.
        /// If the end of the stream is reached, the function will return <c>true</c>, a null pointer, and a count of zero;
        /// a subsequent read will not succeed.
        /// </remarks>
        bool acquire(_Out_ _CharType *&ptr, _Out_ size_t &count) override {
			ptr = nullptr;
			count = 0;
            std::cout << "acquire" << std::endl;
            return true;
        };

        /// <summary>
        /// Releases a block of data acquired using <see cref="::acquire method"/>. This frees the stream buffer to
        /// de-allocate the memory, if it so desires. Move the read position ahead by the count.
        /// </summary>
        /// <param name="ptr">A pointer to the block of data to be released.</param>
        /// <param name="count">The number of characters that were read.</param>
        void release(_Out_writes_(count) _CharType *ptr, _In_ size_t count) override {
            std::cout << "release" << std::endl;
        };

        /// <summary>
        /// Retrieves the stream buffer exception_ptr if it has been set.
        /// </summary>
        /// <returns>Pointer to the exception, if it has been set; otherwise, <c>nullptr</c> will be returned</returns>
        [[nodiscard]] std::exception_ptr exception() const override {
            // std::cout << "exception" << std::endl;
            return nullptr;
        };
    private:
        std::basic_filebuf<_CharType> buf;
        std::atomic_bool _is_open = false;
        std::atomic_bool _can_write = false;
		std::mutex _buffer_mutex;
		std::vector<_CharType> _buffer = std::vector<_CharType>();
        pos_type current_pos;
		// bool _support_range;
		// int64_t _end_index;
    };
}

#endif //QINGZHENYUN_LITE_BASIC_FILE_BUF_H
