/*
 * Copyright (C) 2013 Naver Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "poll.h"

#if defined(WIN32) || defined(_WINDOWS)

#include "win/unixfd.h"

int poll(struct pollfd * fds, nfds_t nfds, long timeout)
{
    size_t bytes = sizeof(pollfd) * nfds;
    pollfd* _fds = (pollfd*)::malloc(bytes);
    memcpy(_fds, fds, bytes);

    for (nfds_t i = 0; i < nfds; ++i) {
        UnixFD* ufd = UnixFD::get(_fds[i].fd);
        ASSERT(ufd->isSocket());
        _fds[i].fd = (SOCKET)ufd->osHandle();
    }

    int retval = FORWARD_CALL(WSAPOLL)(_fds, nfds, timeout);

    for (nfds_t i = 0; i < nfds; ++i) {
        fds[i].revents = _fds[i].revents;
    }

    ::free(_fds);

    return retval;
}

#endif
