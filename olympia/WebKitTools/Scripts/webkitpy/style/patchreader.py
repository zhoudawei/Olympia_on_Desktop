# Copyright (C) 2010 Google Inc. All rights reserved.
# Copyright (C) 2010 Chris Jerdonek (chris.jerdonek@gmail.com)
# Copyright (C) 2010 ProFUSION embedded systems
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import logging

from webkitpy.common.checkout.diff_parser import DiffParser


_log = logging.getLogger("webkitpy.style.patchreader")


class PatchReader(object):

    """Supports checking style in patches."""

    def __init__(self, text_file_reader):
        """Create a PatchReader instance.

        Args:
          text_file_reader: A TextFileReader instance.

        """
        self._text_file_reader = text_file_reader

    def check(self, patch_string):
        """Check style in the given patch."""
        patch_files = DiffParser(patch_string.splitlines()).files

        # The diff variable is a DiffFile instance.
        for path, diff in patch_files.iteritems():
            line_numbers = set()
            for line in diff.lines:
                # When deleted line is not set, it means that
                # the line is newly added (or modified).
                if not line[0]:
                    line_numbers.add(line[1])

            _log.debug('Found %s new or modified lines in: %s'
                       % (len(line_numbers), path))

            # If line_numbers is empty, the file has no new or
            # modified lines.  In this case, we don't check the file
            # because we'll never output errors for the file.
            # This optimization also prevents the program from exiting
            # due to a deleted file.
            if line_numbers:
                self._text_file_reader.process_file(file_path=path,
                                                    line_numbers=line_numbers)
