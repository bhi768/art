/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "jni.h"
#include "object_lock.h"
#include "scoped_thread_state_change-inl.h"

namespace art {

namespace {

extern "C" JNIEXPORT void JNICALL Java_Main_nativeClearResolvedTypes(JNIEnv*, jclass, jclass cls) {
  ScopedObjectAccess soa(Thread::Current());
  mirror::DexCache* dex_cache = soa.Decode<mirror::Class>(cls)->GetDexCache();
  for (size_t i = 0, num_types = dex_cache->NumResolvedTypes(); i != num_types; ++i) {
    dex_cache->SetResolvedType(dex::TypeIndex(i), ObjPtr<mirror::Class>(nullptr));
  }
}

extern "C" JNIEXPORT void JNICALL Java_Main_nativeSkipVerification(JNIEnv*, jclass, jclass cls) {
  ScopedObjectAccess soa(Thread::Current());
  StackHandleScope<1> hs(soa.Self());
  Handle<mirror::Class> klass = hs.NewHandle(soa.Decode<mirror::Class>(cls));
  mirror::Class::Status status = klass->GetStatus();
  if (status == mirror::Class::kStatusResolved) {
    ObjectLock<mirror::Class> lock(soa.Self(), klass);
    klass->SetStatus(klass, mirror::Class::kStatusVerified, soa.Self());
  } else {
    LOG(ERROR) << klass->PrettyClass() << " has unexpected status: " << status;
  }
}

}  // namespace

}  // namespace art