/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FaceTool.h"

#include "FloatType.h"

#include <kdl/memory_utils.h>
#include <kdl/string_format.h>

namespace TrenchBroom {
namespace View {
FaceTool::FaceTool(std::weak_ptr<MapDocument> document)
  : VertexToolBase(document)
  , m_faceHandles(std::make_unique<FaceHandleManager>()) {}

std::vector<Model::BrushNode*> FaceTool::findIncidentBrushes(const vm::polygon3& handle) const {
  return findIncidentBrushes(*m_faceHandles, handle);
}

void FaceTool::pick(
  const vm::ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult) const {
  m_faceHandles->pickCenterHandle(pickRay, camera, pickResult);
}

FaceHandleManager& FaceTool::handleManager() {
  return *m_faceHandles;
}

const FaceHandleManager& FaceTool::handleManager() const {
  return *m_faceHandles;
}

std::tuple<vm::vec3, vm::vec3> FaceTool::handlePositionAndOffset(
  const std::vector<Model::Hit>& hits) const {
  assert(!hits.empty());

  const auto& hit = hits.front();
  assert(hit.hasType(FaceHandleManager::HandleHitType));

  const auto position = hit.target<vm::polygon3>().center();
  return {position, hit.hitPoint() - position};
}

FaceTool::MoveResult FaceTool::move(const vm::vec3& delta) {
  auto document = kdl::mem_lock(m_document);

  auto handles = m_faceHandles->selectedHandles();
  if (document->moveFaces(std::move(handles), delta)) {
    m_dragHandlePosition = m_dragHandlePosition.translate(delta);
    return MoveResult::Continue;
  }
  return MoveResult::Deny;
}

std::string FaceTool::actionName() const {
  return kdl::str_plural(m_faceHandles->selectedHandleCount(), "Move Face", "Move Faces");
}

void FaceTool::removeSelection() {
  const auto handles = m_faceHandles->selectedHandles();
  auto vertexPositions = std::vector<vm::vec3>{};
  vm::polygon3::get_vertices(
    std::begin(handles), std::end(handles), std::back_inserter(vertexPositions));

  const auto commandName =
    kdl::str_plural(handles.size(), "Remove Brush Face", "Remove Brush Faces");
  kdl::mem_lock(m_document)->removeVertices(commandName, std::move(vertexPositions));
}
} // namespace View
} // namespace TrenchBroom
