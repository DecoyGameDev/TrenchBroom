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

#pragma once

#include "Macros.h"

#include <memory>
#include <string>

namespace TrenchBroom {
class Logger;

namespace Assets {
class Texture;
}

namespace IO {
class File;
class FileSystem;
class Path;

class TextureReader {
public:
  class NameStrategy {
  protected:
    NameStrategy();

  public:
    virtual ~NameStrategy();

    NameStrategy* clone() const;

    std::string textureName(const std::string& textureName, const Path& path) const;

  private:
    virtual NameStrategy* doClone() const = 0;
    virtual std::string doGetTextureName(
      const std::string& textureName, const Path& path) const = 0;

    deleteCopyAndMove(NameStrategy);
  };

  class TextureNameStrategy : public NameStrategy {
  public:
    TextureNameStrategy();

  private:
    NameStrategy* doClone() const override;
    std::string doGetTextureName(const std::string& textureName, const Path& path) const override;

    deleteCopyAndMove(TextureNameStrategy);
  };

  /**
   * Determines a texture name from a path removing a prefix of the path and returning the remaining
   * suffix as a string, with the extension removed.
   *
   * Note that the length of a prefix refers to the number of path components and not to the number
   * of characetrs.
   *
   * For example, given the path /this/that/over/here/texture.png and a prefix length of 3, this
   * strategy will return here/texture as the texture name.
   *
   * Given a path with fewer than or the same number of components as the prefix length, an empty
   * string is returned.
   */
  class PathSuffixNameStrategy : public NameStrategy {
  private:
    size_t m_prefixLength;

  public:
    PathSuffixNameStrategy(size_t prefixLength);

  private:
    NameStrategy* doClone() const override;
    std::string doGetTextureName(const std::string& textureName, const Path& path) const override;

    deleteCopyAndMove(PathSuffixNameStrategy);
  };

  class StaticNameStrategy : public NameStrategy {
  private:
    std::string m_name;

  public:
    explicit StaticNameStrategy(const std::string& name);

  private:
    NameStrategy* doClone() const override;
    std::string doGetTextureName(const std::string& textureName, const Path& path) const override;

    deleteCopyAndMove(StaticNameStrategy);
  };

private:
  NameStrategy* m_nameStrategy;

protected:
  const FileSystem& m_fs;
  Logger& m_logger;

protected:
  explicit TextureReader(const NameStrategy& nameStrategy, const FileSystem& fs, Logger& logger);

public:
  virtual ~TextureReader();

  /**
   * Loads a texture from the given file and returns it. If an error occurs while loading the
   * texture, the default texture is returned.
   *
   * @param file the file containing the texture
   * @return an Assets::Texture object
   */
  Assets::Texture readTexture(std::shared_ptr<File> file) const;

protected:
  std::string textureName(const std::string& textureName, const Path& path) const;
  std::string textureName(const Path& path) const;

private:
  /**
   * Loads a texture and returns an Assets::Texture object allocated with new. Should not throw
   * exceptions to report errors loading textures except for unrecoverable errors (out of memory,
   * bugs, etc.).
   *
   * @param file the file containing the texture
   * @return an Assets::Texture object
   */
  virtual Assets::Texture doReadTexture(std::shared_ptr<File> file) const = 0;

protected:
  static bool checkTextureDimensions(size_t width, size_t height);

public:
  static size_t mipSize(size_t width, size_t height, size_t mipLevel);

  deleteCopyAndMove(TextureReader);
};
} // namespace IO
} // namespace TrenchBroom
