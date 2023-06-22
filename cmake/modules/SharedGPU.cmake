# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

if (USE_SHARED_GPU)
  message(STATUS "Build with Shared GPU")
  file_glob_append(RUNTIME_SHARED_GPU_SRCS src/runtime/shared_gpu/*.cc)
    list(APPEND RUNTIME_SRCS ${RUNTIME_SHARED_GPU_SRCS})
endif(USE_SHARED_GPU)


if (USE_GRPC)

  add_definitions(-DWITH_GRPC=1)
  file_glob_append(RUNTIME_GRPC_SRCS src/runtime/grpc/*.cc)
  list(APPEND RUNTIME_SRCS ${RUNTIME_GRPC_SRCS})
endif(USE_GRPC)