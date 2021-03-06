//===-- StreamFile.cpp ------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/Core/StreamFile.h"
#include "lldb/Host/FileSystem.h"
#include "lldb/Utility/Log.h"

#include <stdio.h>

using namespace lldb;
using namespace lldb_private;

// StreamFile constructor
StreamFile::StreamFile() : Stream() { m_file_sp = std::make_shared<File>(); }

StreamFile::StreamFile(uint32_t flags, uint32_t addr_size, ByteOrder byte_order)
    : Stream(flags, addr_size, byte_order) {
  m_file_sp = std::make_shared<File>();
}

StreamFile::StreamFile(int fd, bool transfer_ownership) : Stream() {
  m_file_sp =
      std::make_shared<NativeFile>(fd, File::eOpenOptionWrite, transfer_ownership);
}

StreamFile::StreamFile(FILE *fh, bool transfer_ownership) : Stream() {
  m_file_sp = std::make_shared<NativeFile>(fh, transfer_ownership);
}

StreamFile::StreamFile(const char *path) : Stream() {
  auto file = FileSystem::Instance().Open(
      FileSpec(path), File::eOpenOptionWrite | File::eOpenOptionCanCreate |
                          File::eOpenOptionCloseOnExec);
  if (file)
    m_file_sp = std::move(file.get());
  else {
    // TODO refactor this so the error gets popagated up instead of logged here.
    LLDB_LOG_ERROR(GetLogIfAllCategoriesSet(LIBLLDB_LOG_HOST), file.takeError(),
                   "Cannot open {1}: {0}", path);
    m_file_sp = std::make_shared<File>();
  }
}

StreamFile::StreamFile(const char *path, uint32_t options, uint32_t permissions)
    : Stream() {
  auto file = FileSystem::Instance().Open(FileSpec(path), options, permissions);
  if (file)
    m_file_sp = std::move(file.get());
  else {
    // TODO refactor this so the error gets popagated up instead of logged here.
    LLDB_LOG_ERROR(GetLogIfAllCategoriesSet(LIBLLDB_LOG_HOST), file.takeError(),
                   "Cannot open {1}: {0}", path);
    m_file_sp = std::make_shared<File>();
  }
}

StreamFile::~StreamFile() {}

void StreamFile::Flush() { m_file_sp->Flush(); }

size_t StreamFile::WriteImpl(const void *s, size_t length) {
  m_file_sp->Write(s, length);
  return length;
}
