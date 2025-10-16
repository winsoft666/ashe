/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ASHE_BUFFER_QUEUE_HPP__
#define ASHE_BUFFER_QUEUE_HPP__
#pragma once

#include "ashe/config.h"
#include <mutex>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "ashe/macros.h"

namespace ashe {
typedef struct QueueElem {
    void* dataStartAddress;  // start address of the data that we allocated.
    void* dataReadAddress;   // address of the data in buffer. Next time, we get data from this address.
    size_t size;             // the size of the data.
    struct QueueElem* prev;
    struct QueueElem* next;
} QUEUE_ELEMENT;

// 使用双向链表实现的线程安全的缓冲区队列
//
class ASHE_API BufferQueue {
   public:
    explicit BufferQueue(const std::string& name = "");
    ~BufferQueue();

    std::string getQueueName() const;

    // 将元素放置到队列首部，元素中存放 pData 指向的数据
    // BufferQueue将分配缓冲区来保存 pData，因此在调用后可以释放 pData
    //
    bool pushElementToFront(void* pData, size_t nDataSize);

    // 将元素放置到队列尾部，元素中存放 pData 指向的数据
    // BufferQueue将分配缓冲区来保存 pData，因此在调用后可以释放 pData
    //
    bool pushElementToLast(void* pData, size_t nDataSize);

    // 弹出队列首部元素并返回该元素中的数据，由调用方负责分配和释放 pBuffer
    // 如果元素中的数据大小大于 nBufferSize，则只复制 nBufferSize 字节到 pBuffer；如果nBufferSize为0，则不复制数据
    // 
    // 返回实际复制到 pBuffer 的字节数
    //
    size_t popElementFromFront(void* pBuffer, size_t nBufferSize);

    // 弹出队列尾部元素并返回该元素中的数据，由调用方负责分配和释放 pBuffer
    // 如果元素中的数据大小大于 nBufferSize，则只复制 nBufferSize 字节到 pBuffer；如果nBufferSize为0，则不复制数据
    //
    // 返回实际复制到 pBuffer 的字节数
    //
    size_t popElementFromLast(void* pBuffer, size_t nBufferSize);

    // 拷贝队列首部元素的数据到 pBuffer，由调用方分配和释放 pBuffer
    // 实际复制数据大小为 min(元素数据大小, nBufferSize)
    //
    // 返回实际复制到 pBuffer 的字节数
    //
    size_t getDataFromFrontElement(void* pBuffer, size_t nBufferSize);

    // 拷贝队列尾部元素的数据到 pBuffer，由调用方分配和释放 pBuffer
    // 实际复制数据大小为 min(元素数据大小, nBufferSize)
    //
    // 返回实际复制到 pBuffer 的字节数
    //
    size_t getDataFromLastElement(void* pBuffer, size_t nBufferSize);

    // 清空并释放所有元素
    // 返回被释放的元素总数
    //
    size_t clear();

    size_t getElementCount();

    size_t getTotalDataSize();

    size_t popDataCrossElement(void* pOutputBuffer, size_t nBytesToRead, size_t* pBufferIsThrown);

    // 从队列中移除指定字节数的数据，可能会跨越多个元素
    // 如元素1有100字节，元素2有200字节，调用 removeData(150) 将移除元素1和元素2的前50字节
    //
    // 返回被移除数据所跨越的元素总数
    //
    size_t removeData(size_t nBytesToRemove);

    size_t getFrontElementDataSize();

    size_t getLastElementDataSize();

    // 将队列中所有数据合并到一个连续的缓冲区中
    // BufferQueue分配缓冲区，但由调用方释放该缓冲区
    // 返回缓冲区大小
    //
    size_t toOneBuffer(char** ppBuf);

    // 与toOneBuffer类似，但在缓冲区末尾添加一个空字符('\0')
    //
    size_t toOneBufferWithNullEnding(char** ppBuf);

   private:
    QUEUE_ELEMENT* first_element_;
    QUEUE_ELEMENT* last_element_;
    size_t element_num_;
    size_t total_data_size_;
    std::string queue_name_;
    std::recursive_mutex queue_mutex_;
    ASHE_DISALLOW_COPY_AND_MOVE(BufferQueue);
};
}  // namespace ashe

#endif  //! ASHE_BUFFER_QUEUE_HPP__