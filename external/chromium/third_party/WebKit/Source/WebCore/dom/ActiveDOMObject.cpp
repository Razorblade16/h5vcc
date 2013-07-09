/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 */

#include "config.h"
#include "ActiveDOMObject.h"

#include "ScriptExecutionContext.h"
#include "WebCoreMemoryInstrumentation.h"
#include "WorkerContext.h"
#include "WorkerThread.h"

namespace WebCore {

ActiveDOMObject::ActiveDOMObject(ScriptExecutionContext* scriptExecutionContext, void* upcastPointer)
    : ContextDestructionObserver(scriptExecutionContext)
    , m_pendingActivityCount(0)
#if !ASSERT_DISABLED
    , m_suspendIfNeededCalled(false)
#endif
{
    if (!m_scriptExecutionContext)
        return;

    ASSERT(m_scriptExecutionContext->isContextThread());
    m_scriptExecutionContext->didCreateActiveDOMObject(this, upcastPointer);
}

ActiveDOMObject::~ActiveDOMObject()
{
    if (!m_scriptExecutionContext)
        return;

    ASSERT(m_suspendIfNeededCalled);
    ASSERT(m_scriptExecutionContext->isContextThread());
    m_scriptExecutionContext->willDestroyActiveDOMObject(this);
}

void ActiveDOMObject::suspendIfNeeded()
{
#if !ASSERT_DISABLED
    ASSERT(!m_suspendIfNeededCalled);
    m_suspendIfNeededCalled = true;
#endif
    if (!m_scriptExecutionContext)
        return;

    m_scriptExecutionContext->suspendActiveDOMObjectIfNeeded(this);
}

bool ActiveDOMObject::hasPendingActivity() const
{
    return m_pendingActivityCount;
}

bool ActiveDOMObject::canSuspend() const
{
    return false;
}

void ActiveDOMObject::suspend(ReasonForSuspension)
{
}

void ActiveDOMObject::resume()
{
}

void ActiveDOMObject::stop()
{
}

void ActiveDOMObject::reportMemoryUsage(MemoryObjectInfo* memoryObjectInfo) const
{
    MemoryClassInfo info(memoryObjectInfo, this, WebCoreMemoryTypes::DOM);
}


} // namespace WebCore